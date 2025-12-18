#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"
#include "..\lib\API.h"


int cmd_otf(void) {
    if (LCR_SetMode(3) < 0) {
        printf("ERROR: Cannot set OTF mode\n");
        return -1;
    }
    printf("Switched to OTF mode\n");
    return 0;
}

int cmd_disable(void) {
    if (LCR_SetMode(0) < 0) {
        printf("ERROR: Cannot disable pattern mode\n");
        return -1;
    }
    printf("Switched to disabled mode\n");
    return 0;
}

int cmd_mode(void) {
    API_DisplayMode_t mode;
    
    if (LCR_GetMode(&mode) < 0) {
        printf("ERROR: Cannot read mode\n");
        return -1;
    }
    
    printf("Current Mode: ");
    switch(mode) {
        case PTN_MODE_DISABLE:
            printf("Disabled (0)\n");
            break;
        case PTN_MODE_SPLASH:
            printf("Splash - (1)\n"); // Not needed but for completeness
            break;
        case PTN_MODE_VIDEO:
            printf("Video - (2)\n"); // Not needed but for completeness
            break;
        case PTN_MODE_OTF:
            printf("OTF - (3)\n");
            break;
        default:
            printf("Unknown (%d)\n", mode);
            break;
    }
    return 0;
}

int cmd_pattern(void) {
    printf("Setting up pattern sequence...\n");
    
    cmd_otf();
    
    LCR_ClearPatLut();
    
    if (LCR_AddToPatLut(0, 1000000, 1, 8, 7, 0, 0, 0, 0, 0) < 0) {
        printf("ERROR: Cannot add pattern to LUT\n");
        return -1;
    }
    printf("  Added pattern to LUT\n");
    
    if (LCR_SendPatLut() < 0) {
        printf("ERROR: Cannot send pattern LUT\n");
        return -1;
    }
    printf("  Pattern LUT uploaded\n");
    
    if (LCR_SetPatternConfig(1, 1) < 0) {
        printf("ERROR: Cannot set pattern config\n");
        return -1;
    }
    
    if (LCR_PatternDisplay(0x2) < 0) {
        printf("ERROR: Cannot start pattern\n");
        return -1;
    }
    printf("Pattern sequence STARTED\n");
    
    return 0;
}

int cmd_clear_pattern(void) {
    if (LCR_PatternDisplay(0x0) < 0) {
        printf("ERROR: Cannot stop pattern\n");
        return -1;
    }
    printf("Pattern display STOPPED\n");
    return 0;
}
