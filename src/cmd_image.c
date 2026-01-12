/**
 * cmd_image.c
 * BMP image loading and display commands for DLPC900
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

/**
 * Load a 1-bit BMP file and return the 24-bit structure
 * @param filename - Path to the BMP file
 * @return Pointer to Image_t structure (24-bit) or NULL on failure
 */
static Image_t* load_bmp_file(const char *filename) {
    Image_t imgInfo;
    Image_t *image = NULL;
    
    if (BMP_GetFileInfo(filename, &imgInfo) < 0) {
        printf("ERROR: Cannot read BMP file: %s\n", filename);
        return NULL;
    }
    
    if (imgInfo.BitDepth != 1) {
        printf("ERROR: Only 1-bit BMP images are supported (got %d-bit)\n", imgInfo.BitDepth);
        return NULL;
    }
    
    /* 
     * Note: Allocate as 24-bit
     * BMP_ParseImage() inside BMP_AllocImage() reads 1-bit BMP and converts to output 24-bit depth via palette
     */
    image = BMP_AllocImage(imgInfo.Width, imgInfo.Height, 24);
    if (!image) {
        printf("ERROR: Cannot allocate image buffer\n");
        return NULL;
    }

    // Show BMP metadata
    printf("\n=== BMP Image Metadata ===\n");
    printf("  Filename:   %s\n", filename);
    printf("  Width:      %d pixels\n", imgInfo.Width);
    printf("  Height:     %d pixels\n", imgInfo.Height);
    printf("  Input:      %d-bit BMP\n", imgInfo.BitDepth);
    printf("  Output:     24-bit (for splash format)\n");
    printf("  LineWidth:  %d bytes\n", image->LineWidth);
    printf("==========================\n\n");
    
    if (BMP_LoadFromFile(filename, image) < 0) {
        printf("ERROR: Cannot load BMP data\n");
        BMP_FreeImage(image);
        return NULL;
    }

    // ASCII preview (60x20)
    printf("=== Image Preview (scaled 60x20) ===\n");
    for (int py = 0; py < 20; py++) {
        int imgY = (py * image->Height) / 20;
        for (int px = 0; px < 60; px++) {
            int imgX = (px * image->Width) / 60;
            int idx = imgY * image->LineWidth + (imgX * 3);
            printf("%c", image->Buffer[idx] ? '#' : '.');
        }
        printf("\n");
    }
    printf("====================================\n\n");

    printf("BMP loaded successfully (24-bit)\n");
    return image;
}

/**
 * Convert a 24-bit Image_t to splash format with RLE compression
 * @param image - Pointer to 24-bit Image_t
 * @param outSplash - Pointer to store allocated splash buffer
 * @return Size of splash data in bytes, or -1 on failure
 */
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

/**
 * Upload splash pattern data to DMD at given image index
 * @param splash - Pointer to splash data
 * @param splashSize - Size of splash data in bytes
 * @param imageIndex - Image index on DMD to upload to
 * @return 0 on success, -1 on failure
 */
static int upload_pattern_data(uint08 *splash, int splashSize, int imageIndex) {
    int offset, chunkSize;
    
    printf("  Uploading to image index %d...\n", imageIndex);
    
    if (LCR_InitPatternMemLoad(TRUE, imageIndex, splashSize) < 0) {
        printf("ERROR: Cannot init pattern upload\n");
        return -1;
    }
    
    // Note: Max 504 bytes per USB packet
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

/**
 * Start pattern display on DMD with given parameters. Helper function to 
 * @param exposureUs - Exposure time in microseconds
 * @param bitDepth - Bit depth (1 or 8)
 * @param ledSelect - LED select bitmask
 *                  0 = No LED (Pass Through)
 *                  1 = Red
 *                  2 = Green
 *                  3 = Yellow (Green + Red)
 *                  4 = Blue
 *                  5 = Magenta (Blue + Red)
 *                  6 = Cyan (Blue + Green)
 *                  7 = White (Red + Blue + Green)
 * @param repeat - Number of times to repeat the pattern
 * @param splashIdx - Splash image index to display
 * @return 0 on success, -1 on failure
 */
static int start_pattern_display(int exposureUs, int bitDepth, int ledSelect, unsigned int repeat, int splashIdx) {
    LCR_PatternDisplay(0);
    LCR_ClearPatLut();
    
    if (LCR_AddToPatLut(0, exposureUs, 0, bitDepth, ledSelect, 0, 0, 0, splashIdx, 0) < 0) {
        printf("ERROR: Cannot add to pattern LUT\n");
        return -1;
    }
    printf("  LUT entry: exposure=%dus, bitDepth=%d, LED=0x%X, splashIdx=%d\n", exposureUs, bitDepth, ledSelect, splashIdx);
    
    
    if (LCR_OpenMailbox(2) < 0) {
        printf("ERROR: Cannot open mailbox\n");
        return -1;
    }
    
    printf("  Mailbox opened\n");

    if (LCR_MailboxSetAddr(0) < 0) {
        printf("ERROR: Cannot set mailbox address\n");
        LCR_CloseMailbox();
        return -1;
    }
    
    printf("  Mailbox address set\n");

    if (LCR_SendPatLut() < 0) {
        printf("ERROR: Cannot send pattern LUT\n");
        LCR_CloseMailbox();
        return -1;
    }
    printf("  Pattern LUT sent\n");
    
    if (LCR_CloseMailbox() < 0) {
        printf("ERROR: Cannot close mailbox\n");
        return -1;
    }

    if (LCR_SetPatternConfig(1, repeat) < 0) {
        printf("ERROR: Cannot set pattern config\n");
        return -1;
    }
    printf("  Pattern config set (entries=1, repeat=%d)\n", repeat);
    
    if (LCR_PatternDisplay(1) < 0) {
        printf("ERROR: Pattern validation failed\n");
        return -1;
    }
    printf("  Pattern validated\n");

    if (LCR_PatternDisplay(2) < 0) {
        printf("ERROR: Cannot start pattern display\n");
        return -1;
    }
    
    printf("  Pattern display STARTED\n");
    return 0;
}

/**
 * Load BMP file, convert to splash, upload and display on DMD
 * @param filename - Path to BMP file
 * @return 0 on success, -1 on failure 
 */
static int display_bmp(const char *filename) {
    Image_t *image = NULL;
    uint08 *splash = NULL;
    int splashSize;
    int result = -1;
    int imageIndex = 0;
    
    printf("\n=== Loading BMP to DMD ===\n\n");
    
    printf("[1] Switching to OTF mode...\n");
    if (cmd_otf() < 0) {
        printf("ERROR: Failed to switch to OTF mode\n");
        goto cleanup;
    }
    
    printf("\n[2] Enabling LEDs...\n");
    if (LCR_SetLedEnables(1, 1, 1, 1) < 0) {
        printf("WARNING: Could not enable LEDs\n");
    } else {
        printf("  LEDs enabled\n");
    }

    printf("\n[3] Loading BMP file...\n");
    image = load_bmp_file(filename);
    if (!image) goto cleanup;
    
    printf("\n[4] Converting to splash format...\n");
    splashSize = convert_to_splash(image, &splash);
    if (splashSize < 0) goto cleanup;
    
    printf("\n[5] Uploading pattern data...\n");
    if (upload_pattern_data(splash, splashSize, imageIndex) < 0) goto cleanup;
    
    /* exposure = 500ms, repeat=0xFFFFFFFF for infinite loop */
    printf("\n[6] Starting pattern display...\n");
    if (start_pattern_display(500000, 1, 7, 0xFFFFFFFF, imageIndex) < 0) goto cleanup;
    
    result = 0;
    
cleanup:
    if (image) BMP_FreeImage(image);
    if (splash) SPL_Free(splash);
    return result;
}


static int cmd_get_bmp(char *filename, int maxLen) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("Enter BMP filename: ");
    fflush(stdout);
    if (fgets(filename, maxLen, stdin) == NULL) {
        return -1;
    }
    filename[strcspn(filename, "\r\n")] = 0;
    return 0;
}

int cmd_load_bmp(void) {
    char filename[256];
    
    if (cmd_get_bmp(filename, sizeof(filename)) < 0) return -1;
    if (display_bmp(filename) < 0) return -1;
        
    printf("\n=== Uploaded image displayed on DMD ===\n");
    return 0;
}

int cmd_load_half(void) {
    if(display_bmp("test_patterns\\testBMP.bmp") < 0) return -1;
    printf("\n=== Half-white and half-black image displayed on DMD ===\n");
    return 0;
}

int cmd_load_white(void) {
    if(display_bmp("test_patterns\\testBMPwhite.bmp") < 0) return -1;
    printf("\n=== White image displayed on DMD ===\n");
    return 0;
}

int cmd_load_black(void) {
    if(display_bmp("test_patterns\\testBMPblack.bmp") < 0) return -1;

    printf("\n=== Black image displayed on DMD ===\n");
    return 0;
}

