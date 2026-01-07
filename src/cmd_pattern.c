/*
* Pattern commands and testing LED display
* Only uses OTF and Disable modes
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"
#include "..\lib\API.h"

/**
* Switches to On-The-Fly (OTF) pattern mode
*/
int cmd_otf(void) {
    if (LCR_SetMode(3) < 0) {
        printf("ERROR: Cannot set OTF mode\n");
        return -1;
    }
    printf("Switched to OTF mode\n");
    return 0;
}

/**
* Switches to Disable pattern mode
*/
int cmd_disable(void) {
    if (LCR_SetMode(0) < 0) {
        printf("ERROR: Cannot disable pattern mode\n");
        return -1;
    }
    printf("Switched to disabled mode\n");
    return 0;
}

/**
* Prints out the current pattern mode
*/
int cmd_pattern_mode(void) {
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

/**
* Stop current pattern sequence from running
*/
int cmd_clear_pattern(void) {
    if (LCR_PatternDisplay(0x0) < 0) {
        printf("ERROR: Cannot stop pattern\n");
        return -1;
    }
    printf("Pattern display STOPPED\n");
    return 0;
}

/**
* Test if pattern sequence works by generating a checkerboard
*/
int cmd_tpg(void) {
    cmd_disable();
    
    /* Set input source to internal test pattern (0=parallel, 1=internal test pattern) */
    if (LCR_SetInputSource(1, 0) < 0) {
        printf("ERROR: Cannot set input source to TPG\n");
        return -1;
    }
    
    /* Select test pattern: 
        0=solid, 
        1=horizontal ramp, 
        2=vertical ramp,
        3=horizontal lines, 
        4=diagonal lines, 
        5=vertical lines,
        6=grid, 
        7=checkerboard, 
        8=RGB ramp, 
        9=color bars */
    if (LCR_SetTPGSelect(7) < 0) {  
        printf("ERROR: Cannot select TPG pattern\n");
        return -1;
    }
    
    printf("TPG checkerboard pattern displayed on DMD\n");
    return 0;
}

