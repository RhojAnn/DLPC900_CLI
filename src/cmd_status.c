#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\lib\API.h"
#include "usb.h"

int cmd_status(void) {
    unsigned char hw, sys, main_status, dlpa, dmd;
    
    if (LCR_GetStatus(&hw, &sys, &main_status, &dlpa, &dmd) < 0) {
        printf("ERROR: Cannot read status\n");
        return -1;
    }
    
    printf("Device Status:\n");
    printf("  Hardware:       0x%02X\n", hw);
    printf("  System:         0x%02X\n", sys);
    printf("  Main:           0x%02X\n", main_status);
    printf("  DLPA200:        0x%02X\n", dlpa);
    printf("  DMD Connection: 0x%02X\n", dmd);
    return 0;
}

int cmd_version(void) {
    unsigned int app, api, swconfig, seqconfig;
    
    if (LCR_GetVersion(&app, &api, &swconfig, &seqconfig) < 0) {
        printf("ERROR: Cannot read version\n");
        return -1;
    }
    
    printf("Firmware Version:\n");
    printf("  App:       %d.%d.%d\n", (app >> 24) & 0xFF, (app >> 16) & 0xFF, app & 0xFFFF);
    printf("  API:       %d.%d.%d\n", (api >> 24) & 0xFF, (api >> 16) & 0xFF, api & 0xFFFF);
    printf("  SW Config: %d.%d.%d\n", (swconfig >> 24) & 0xFF, (swconfig >> 16) & 0xFF, swconfig & 0xFFFF);
    printf("  Seq Config:%d.%d.%d\n", (seqconfig >> 24) & 0xFF, (seqconfig >> 16) & 0xFF, seqconfig & 0xFFFF);
    return 0;
}

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

int cmd_solid(void) {
    if (LCR_SetMode(PTN_MODE_DISABLE) < 0) {
        printf("ERROR: Cannot disable pattern mode\n");
        return -1;
    }
    
    if (LCR_SetInputSource(1, 0) < 0) {
        printf("ERROR: Cannot set input source\n");
        return -1;
    }
    
    if (LCR_SetTPGSelect(0) < 0) {
        printf("ERROR: Cannot select solid pattern\n");
        return -1;
    }
    
    if (LCR_SetTPGColor(1023, 1023, 1023, 0, 0, 0) < 0) {
        printf("ERROR: Cannot set TPG color\n");
        return -1;
    }
    
    printf("Solid black pattern displayed - all mirrors ON\n");
    return 0;
}
