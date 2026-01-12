/**
* Handles USB Connection
*/
#include <stdio.h>
#include "usb.h"

/** 
 *   Establish USB connection to DLPC900 device
*/
int connect_device(void) {
    if (USB_Init() != 0) {
        printf("ERROR: USB initialization failed\n");
        return -1;
    }
    
    if (USB_Open() != 0) {
        printf("ERROR: Cannot connect to DLPC900. Is the device connected?\n");
        USB_Exit();
        return -1;
    }
    
    printf("Connected to DLPC900\n");
    return 0;
}

/*
*   Close USB connection to DLPC900 device
*/
void disconnect_device(void) {
    USB_Close();
    USB_Exit();
}
