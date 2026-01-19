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
 * Gets the current standby delay in seconds
 */
static int get_standby_delay(void){
    unsigned char current_delay;
    
    if(LCR_GetStandbyDelaySec(&current_delay) < 0){
        printf("ERROR: Cannot set standby delay\n");
        return -1;
    }

    // printf("Standby delay: %d seconds\n", current_delay);
    return current_delay;
}

/**
 * Gets the current power mode (standby or normal)
 */
int cmd_get_power_mode(void){

    /*
    int is_idle = LCR_GetDMDSaverMode();

    if(is_idle < 0){
        printf("ERROR: Cannot get power mode\n");
        return -1;
    } else if(is_idle == 1){
        printf("Power mode: Idle (DMD saver mode enabled)\n");
        return 0;
    }
    */

    int is_normal = get_standby_delay();

    if(is_normal < 0){
        printf("ERROR: Cannot get standby delay\n");
        return -1;
    } else if(is_normal != 37){
        printf("Power mode: Normal\n");
        return 0;
    }

    printf("Power mode: Standby\n");
    return 1;
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

/**
 * Set the DMD to standby mode
 */
int cmd_set_standby(void){
    if(get_standby_delay() == -1){
        printf("ERROR: Cannot get standby delay\n");
        return -1;
    }

    if(get_standby_delay() == 37){
        printf("ERROR: Already running standby mode\n");
        return -1;
    }

    if(LCR_SetPowerMode(TRUE) < 0){
        printf("ERROR: Cannot set to standby mode\n");
        return -1;
    }

    return 0;
}

/**
 * Sets the DMD to normal mode
 */
int cmd_set_normal(void){
    if(get_standby_delay() == -1){
        printf("ERROR: Cannot get standby delay\n");
        return -1;
    }

    if(get_standby_delay() == 37){
        printf("ERROR: Running standby mode. Please wait.\n");
        return -1;
    }

    if(LCR_SetPowerMode(FALSE) < 0){
        printf("ERROR: Cannot set to normal mode\n");
        return -1;
    }
    return 0;
}