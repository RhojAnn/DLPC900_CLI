/**
 * cmd_image.c
 * BMP image loading and display commands for DLPC900
 * CURRENTLY NOT WORKING.
 * Will be only accepting 1-bit bmp files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"
#include "API.h"
#include "BMPParser.h"
#include "pattern.h"
#include "splash.h"

// I think some memory location is being misused here but I havent spotted it

static Image_t* load_bmp_file(const char *filename) {
    Image_t imgInfo;
    Image_t *image1bit = NULL;
    Image_t *image24 = NULL;
    
    /* Get image info first */
    if (BMP_GetFileInfo(filename, &imgInfo) < 0) {
        printf("ERROR: Cannot read BMP file: %s\n", filename);
        return NULL;
    }
    
    /* Only accept 1-bit images */
    if (imgInfo.BitDepth != 1) {
        printf("ERROR: Only 1-bit BMP images are supported (got %d-bit)\n", imgInfo.BitDepth);
        return NULL;
    }
    
    /* Allocate and load the 1-bit image */
    image1bit = BMP_AllocImage(imgInfo.Width, imgInfo.Height, imgInfo.BitDepth);
    if (!image1bit) {
        printf("ERROR: Cannot allocate 1-bit image buffer\n");
        return NULL;
    }

    /* Show BMP metadata */
    printf("\n=== BMP Image Metadata ===\n");
    printf("  Filename:   %s\n", filename);
    printf("  Width:      %d pixels\n", image1bit->Width);
    printf("  Height:     %d pixels\n", image1bit->Height);
    printf("  Bit Depth:  %d-bit\n", image1bit->BitDepth);
    printf("  LineWidth:  %d bytes\n", image1bit->LineWidth);
    printf("==========================\n\n");
    
    
    if (BMP_LoadFromFile(filename, image1bit) < 0) {
        printf("ERROR: Cannot load BMP data\n");
        BMP_FreeImage(image1bit);
        return NULL;
    }
    
    /* DEBUG: Check raw bytes loaded from file */
    printf("DEBUG: First 16 bytes of 1-bit buffer: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", image1bit->Buffer[i]);
    }
    printf("\n");
    
    /* Count non-zero bytes */
    int nonZero = 0;
    int totalBytes = image1bit->LineWidth * image1bit->Height;
    for (int i = 0; i < totalBytes; i++) {
        if (image1bit->Buffer[i] != 0) nonZero++;
    }
    printf("DEBUG: Non-zero bytes: %d / %d (%.1f%%)\n\n", 
           nonZero, totalBytes, (100.0 * nonZero) / totalBytes);
    

    printf("1-bit BMP loaded successfully\n");
    
    /* Splash format requires 24-bit RGB - convert 1-bit to 24-bit */
    image24 = BMP_AllocImage(imgInfo.Width, imgInfo.Height, 24);
    if (!image24) {
        printf("ERROR: Cannot allocate 24-bit image buffer\n");
        BMP_FreeImage(image1bit);
        return NULL;
    }
    

    printf("Converting 1-bit to 24-bit for splash format...\n");
    
    /* 
    // Show ASCII preview (first 80x40 pixels only)
    printf("\n=== Image Preview (80x40) ===\n");
    int previewW = (image1bit->Width < 80) ? image1bit->Width : 80;
    int previewH = (image1bit->Height < 40) ? image1bit->Height : 40;
    
    for (int py = 0; py < previewH; py++) {
        for (int px = 0; px < previewW; px++) {
            int byteIdx = py * image1bit->LineWidth + (px / 8);
            int bitIdx = 7 - (px % 8);
            uint08 bit = (image1bit->Buffer[byteIdx] >> bitIdx) & 1;
            printf("%c", bit ? 'X' : '-');
        }
        printf("\n");
    }
    printf("=============================\n\n");
    */

    /* Do the actual conversion (no printing here) */
    int x, y;
    for (y = 0; y < image1bit->Height; y++) {
        for (x = 0; x < image1bit->Width; x++) {
            int srcByteIdx = y * image1bit->LineWidth + (x / 8);
            int srcBitIdx = 7 - (x % 8);
            uint08 pixelVal = (image1bit->Buffer[srcByteIdx] >> srcBitIdx) & 1;
            pixelVal = pixelVal ? 255 : 0;  /* Expand 1-bit to 8-bit */

            //printf("%c", pixelVal ? 'X' : '-');


            /* Write to 24-bit image (RGB) */
            int dstIdx = y * image24->LineWidth + (x * 3);
            image24->Buffer[dstIdx + 0] = pixelVal;  /* R */
            image24->Buffer[dstIdx + 1] = pixelVal;  /* G */
            image24->Buffer[dstIdx + 2] = pixelVal;  /* B */
        }
        printf("\n");
    }
    printf("=============================\n\n");

    BMP_FreeImage(image1bit);
    printf("Conversion complete\n");
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


static int upload_pattern_data(uint08 *splash, int splashSize, int imageIndex) {
    int offset, chunkSize;
    
    printf("  Uploading to image index %d...\n", imageIndex);
    
    if (LCR_InitPatternMemLoad(TRUE, imageIndex, splashSize) < 0) {
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
    
    printf("  Pattern uploaded to DMD (index %d, %d bytes)\n", imageIndex, splashSize);
    return 0;
}


static int start_pattern_display(int exposureUs, int bitDepth, int ledSelect, int repeat, int splashIdx) {
    LCR_PatternDisplay(0x0);
    LCR_ClearPatLut();
    
    /* patNum, exposureUs, clearPat, bitDepth, LED, waitTrig, darkTime, trigOut2, splashIdx, bitIdx */
    if (LCR_AddToPatLut(0, exposureUs, 1, bitDepth, ledSelect, 0, 0, 0, splashIdx, 0) < 0) {
        printf("ERROR: Cannot add to pattern LUT\n");
        return -1;
    }
    printf("  LUT entry: exposure=%dus, bitDepth=%d, LED=0x%X, splashIdx=%d\n", exposureUs, bitDepth, ledSelect, splashIdx);
    
    
    if (LCR_OpenMailbox(2) < 0) {
        printf("ERROR: Cannot open mailbox\n");
        return -1;
    }
    
    if (LCR_MailboxSetAddr(0) < 0) {
        printf("ERROR: Cannot set mailbox address\n");
        LCR_CloseMailbox();
        return -1;
    }
    

    if (LCR_SendPatLut() < 0) {
        printf("ERROR: Cannot send pattern LUT\n");
        LCR_CloseMailbox();
        return -1;
    }
    printf("  Pattern LUT sent\n");
    
    /* Close mailbox */
    LCR_CloseMailbox();
    
    if (LCR_SetPatternConfig(1, repeat) < 0) {
        printf("ERROR: Cannot set pattern config\n");
        return -1;
    }
    printf("  Pattern config set (entries=1, repeat=%d)\n", repeat);
    
    /*

    // Validate the pattern sequence first
    if (LCR_PatternDisplay(0x1) < 0) {
        printf("ERROR: Pattern validation failed\n");
        return -1;
    }
    printf("  Pattern validated\n");
   */

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
    int imageIndex = 0;  /* Use image index 0 */

    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    /* Get filename from user */
    printf("Enter BMP filename: ");
    fflush(stdout);
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        return -1;
    }
    filename[strcspn(filename, "\r\n")] = 0;
    
    
    /* Step 3: Switch to OTF mode and stop current pattern */
    // if (cmd_otf() < 0) goto cleanup;
    // if (cmd_clear_pattern() < 0) goto cleanup;
    
    /* Step 4: Enable LEDs */
    if (LCR_SetLedEnables(1, 1, 1, 1) < 0) {
        printf("WARNING: Could not enable LEDs\n");
    } else {
        printf("LEDs enabled\n");
    }

        /* Step 1: Load BMP file */
    image = load_bmp_file(filename);
    if (!image) goto cleanup;
    
    /* Step 2: Convert to splash format */
    splashSize = convert_to_splash(image, &splash);
    if (splashSize < 0) goto cleanup;
    
    /* Step 5: Upload pattern data */
    if (upload_pattern_data(splash, splashSize, imageIndex) < 0) goto cleanup;
    
    /* Step 6: Configure and start display (1-bit pattern, all LEDs, repeat) */
    if (start_pattern_display(100000, 1, 7, 1, imageIndex) < 0) goto cleanup;
    
    printf("Image displayed on DMD!\n");
    result = 0;
    
cleanup:
    if (image) BMP_FreeImage(image);
    if (splash) SPL_Free(splash);
    return result;
}
