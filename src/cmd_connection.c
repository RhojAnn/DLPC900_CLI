/**
* Handles USB Connection
*/
#include <stdio.h>
#include "usb.h"

/**
 * Checks if DLPC900 device is connected
 */
int is_dmd_connected(void) {
    if (USB_Open() != 0) {
            printf("ERROR: Cannot connect to DLPC900. Is the device connected?\n");
            USB_Exit();
            return -1;
        }
    return 0;
}

/** 
 *   Establish USB connection to DLPC900 device
*/
int dmd_connect(void) {
    if (USB_Init() != 0) {
        printf("ERROR: USB initialization failed\n");
        return -1;
    }
    
    if(is_dmd_connected() != 0) return -1;
    
    printf("Connected to DLPC900\n");
    return 0;
}


/*
*   Close USB connection to DLPC900 device
*/
void dmd_disconnect(void) {
    USB_Close();
    USB_Exit();
}
