/*
 * cmd_image.c
 * BMP image loading and display commands for DLPC900
 * CURRENTLY NOT WORKING. Feature may not be needed depending on the requirements
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"
#include "API.h"
#include "BMPParser.h"
#include "pattern.h"
#include "splash.h"

static Image_t* load_bmp_file(const char *filename) {
    Image_t imgInfo;
    Image_t *image = NULL;
    Image_t *image24 = NULL;
    
    /* Get image info first */
    if (BMP_GetFileInfo(filename, &imgInfo) < 0) {
        printf("ERROR: Cannot read BMP file: %s\n", filename);
        return NULL;
    }
    printf("Image: %dx%d, %d-bit\n", imgInfo.Width, imgInfo.Height, imgInfo.BitDepth);
    
    /* Splash format requires 24-bit RGB images */
    image24 = BMP_AllocImage(imgInfo.Width, imgInfo.Height, 24);
    if (!image24) {
        printf("ERROR: Cannot allocate 24-bit image buffer\n");
        return NULL;
    }
    
    /* If source is not 24-bit, load original first then convert */
    if (imgInfo.BitDepth != 24) {
        printf("Converting %d-bit to 24-bit...\n", imgInfo.BitDepth);
        
        /* Load original bit depth image */
        image = BMP_AllocImage(imgInfo.Width, imgInfo.Height, imgInfo.BitDepth);
        if (!image) {
            printf("ERROR: Cannot allocate image buffer\n");
            BMP_FreeImage(image24);
            return NULL;
        }
        
        if (BMP_LoadFromFile(filename, image) < 0) {
            printf("ERROR: Cannot load BMP data\n");
            BMP_FreeImage(image);
            BMP_FreeImage(image24);
            return NULL;
        }
        
        /* Convert to 24-bit by expanding each pixel */
        int x, y;
        for (y = 0; y < image->Height; y++) {
            for (x = 0; x < image->Width; x++) {
                uint08 pixelVal = 0;
                int srcByteIdx, srcBitIdx;
                int dstIdx;
                
                if (imgInfo.BitDepth == 1) {
                    srcByteIdx = y * image->LineWidth + (x / 8);
                    srcBitIdx = 7 - (x % 8);
                    pixelVal = (image->Buffer[srcByteIdx] >> srcBitIdx) & 1;
                    pixelVal = pixelVal ? 255 : 0;  /* Expand 1-bit to 8-bit */
                } else if (imgInfo.BitDepth == 8) {
                    srcByteIdx = y * image->LineWidth + x;
                    pixelVal = image->Buffer[srcByteIdx];
                } else {
                    pixelVal = 128;
                }
                
                /* Write to 24-bit image (RGB) */
                dstIdx = y * image24->LineWidth + (x * 3);
                image24->Buffer[dstIdx + 0] = pixelVal;  /* R */
                image24->Buffer[dstIdx + 1] = pixelVal;  /* G */
                image24->Buffer[dstIdx + 2] = pixelVal;  /* B */
            }
        }
        
        BMP_FreeImage(image);
        printf("Conversion complete\n");
    } else {
        /* 24-bit image - load directly */
        if (BMP_LoadFromFile(filename, image24) < 0) {
            printf("ERROR: Cannot load BMP data\n");
            BMP_FreeImage(image24);
            return NULL;
        }
    }
    
    printf("BMP loaded successfully (24-bit)\n");
    return image24;
}


static int convert_to_splash(Image_t *image, uint08 **outSplash) {
    uint08 *splash = SPL_AllocSplash(image->Width, image->Height);
    if (!splash) {
        printf("ERROR: Cannot allocate splash buffer\n");
        return -1;
    }
    
    int splashSize = SPL_ConvImageToSplash(image, SPL_COMP_RLE, splash);
    if (splashSize < 0) {
        printf("ERROR: Cannot convert to splash format\n");
        SPL_Free(splash);
        return -1;
    }
    
    printf("Compressed to %d bytes\n", splashSize);
    *outSplash = splash;
    return splashSize;
}


static int upload_pattern_data(uint08 *splash, int splashSize) {
    int offset, chunkSize;
    
    if (LCR_InitPatternMemLoad(TRUE, 0, splashSize) < 0) {
        printf("ERROR: Cannot init pattern upload\n");
        return -1;
    }
    
    /* Max 504 bytes per USB packet */
    offset = 0;
    while (offset < splashSize) {
        chunkSize = (splashSize - offset > 504) ? 504 : (splashSize - offset);
        if (LCR_pattenMemLoad(TRUE, splash + offset, chunkSize) < 0) {
            printf("ERROR: Upload failed at offset %d\n", offset);
            return -1;
        }
        offset += chunkSize;
    }
    
    printf("Pattern uploaded to DMD\n");
    return 0;
}


static int start_pattern_display(int exposureUs, int bitDepth, int ledSelect, int repeat) {
    LCR_ClearPatLut();
    
    /* patNum, exposureUs, clearPat, bitDepth, LED, waitTrig, darkTime, trigOut2, splashIdx, bitIdx */
    if (LCR_AddToPatLut(0, exposureUs, 1, bitDepth, ledSelect, 0, 0, 0, 0, 0) < 0) {
        printf("ERROR: Cannot add to pattern LUT\n");
        return -1;
    }
    printf("  LUT entry added: exposure=%dus, bitDepth=%d, LED=0x%X\n", exposureUs, bitDepth, ledSelect);
    
    if (LCR_SendPatLut() < 0) {
        printf("ERROR: Cannot send pattern LUT\n");
        return -1;
    }
    printf("  Pattern LUT sent\n");
    
    if (LCR_SetPatternConfig(1, repeat) < 0) {
        printf("ERROR: Cannot set pattern config\n");
        return -1;
    }
    printf("  Pattern config set (entries=1, repeat=%d)\n", repeat);
    
    /* Validate the pattern sequence first */
    if (LCR_PatternDisplay(0x1) < 0) {
        printf("ERROR: Pattern validation failed\n");
        return -1;
    }
    printf("  Pattern validated\n");
    
    /* Now start the pattern */
    if (LCR_PatternDisplay(0x2) < 0) {
        printf("ERROR: Cannot start pattern display\n");
        return -1;
    }
    
    printf("Pattern display STARTED\n");
    return 0;
}

// Load BMP image and display on DMD
int cmd_load_bmp(void) {
    char filename[256];
    Image_t *image = NULL;
    uint08 *splash = NULL;
    int splashSize;
    int result = -1;
    
    /* Get filename from user */
    printf("Enter BMP filename: ");
    fflush(stdout);
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        return -1;
    }
    filename[strcspn(filename, "\r\n")] = 0;
    
    /* Step 1: Load BMP file */
    image = load_bmp_file(filename);
    if (!image) goto cleanup;
    
    /* Step 2: Convert to splash format */
    splashSize = convert_to_splash(image, &splash);
    if (splashSize < 0) goto cleanup;
    
    /* Step 3: Switch to OTF mode and stop current pattern */
    if (cmd_otf() < 0) goto cleanup;
    if (cmd_clear_pattern() < 0) goto cleanup;
    
    /* Step 4: Enable LEDs */
    if (LCR_SetLedEnables(1, 1, 1, 1) < 0) {
        printf("WARNING: Could not enable LEDs\n");
    } else {
        printf("LEDs enabled\n");
    }
    
    /* Step 5: Upload pattern data */
    if (upload_pattern_data(splash, splashSize) < 0) goto cleanup;
    
    /* Step 6: Configure and start display */
    if (start_pattern_display(10000000, 8, 7, 1) < 0) goto cleanup;
    
    printf("Image displayed on DMD!\n");
    result = 0;
    
cleanup:
    if (image) BMP_FreeImage(image);
    if (splash) SPL_Free(splash);
    return result;
}
