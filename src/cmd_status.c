/*
* Status and Version commands
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\lib\API.h"
#include "usb.h"

/*
* Prints out the current status for: Hardware, System, Main, DLPA200, DMD Connection
* 
* As of 19/12/2025, DMD Connection Status face detection issues 
* with potential reason of the DMD trying to detect a second controller 
* but it doesn't exists
*/
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

/**
 * Prints out the firmware version information: App, API, SW Config, Seq Config
 */
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

/**
 * Toggles the DMD saver mode (idle mode)
 */
int cmd_toggle_idle(void){
    int current_mode = LCR_GetDMDSaverMode();
    
    if(current_mode < 0){
        printf("ERROR: Cannot read DMD saver mode\n");
        return -1;
    }
    
    unsigned char new_mode = !current_mode;
    
    if(LCR_SetDMDSaverMode(new_mode) < 0){
        printf("ERROR: Cannot set DMD saver mode\n");
        return -1;
    }

    printf("DMD saver mode %s\n", new_mode ? "enabled" : "disabled");
    return 0;
}