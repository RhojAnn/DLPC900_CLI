/*
* DLPC900 Pattern CLI
* Simple command-line interface to control DLPC900 pattern modes
* Will be extended with a python wrapper for GUI use   
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "API.h"
#include "usb.h"
#include "diagnosticwindow.h"

void print_menu(void) {
    printf("\n");
    printf("=== DLPC900 Menu ===");
    printf("\n");
    printf("  1. Get device status\n");
    printf("  2. Get firmware version\n");
    printf("  3. Get current mode\n");
    printf("  4. Run OTF pattern sequence\n");
    printf("  5. Stop pattern display\n");
    printf("  6. Switch to OTF mode\n");
    printf("  7. Disable pattern mode\n");
    printf("  8. Show TPG test pattern (checkerboard)\n");
    printf("  9. Show solid color\n");
    printf("  0. Exit\n");
    printf("\n");
}

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

void disconnect_device(void) {
    USB_Close();
    USB_Exit();
}

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

int cmd_OTF(void) {
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
            printf("Splash - patterns from flash (1)\n");
            break;
        case PTN_MODE_VIDEO:
            printf("Video - patterns via video port (2)\n");
            break;
        case PTN_MODE_OTF:
            printf("OTF - patterns via USB/I2C (3)\n");
            break;
        default:
            printf("Unknown (%d)\n", mode);
            break;
    }
    return 0;
}

int cmd_pattern(void) {
    printf("Setting up pattern sequence...\n");
    
    cmd_OTF();
    
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

int cmd_stop(void) {
    if (LCR_PatternDisplay(0x0) < 0) {
        printf("ERROR: Cannot stop pattern\n");
        return -1;
    }
    printf("Pattern display STOPPED\n");
    return 0;
}

int cmd_tpg(void) {
    /* Disable pattern mode first to use TPG */
    if (LCR_SetMode(PTN_MODE_DISABLE) < 0) {
        printf("ERROR: Cannot disable pattern mode\n");
        return -1;
    }
    
    /* Set input source to internal test pattern (0=parallel, 1=internal test pattern) */
    if (LCR_SetInputSource(1, 0) < 0) {
        printf("ERROR: Cannot set input source to TPG\n");
        return -1;
    }
    
    /* Select test pattern: 0=solid, 1=horizontal ramp, 2=vertical ramp,
       3=horizontal lines, 4=diagonal lines, 5=vertical lines,
       6=grid, 7=checkerboard, 8=RGB ramp, 9=color bars */
    if (LCR_SetTPGSelect(7) < 0) {  /* 7 = checkerboard */
        printf("ERROR: Cannot select TPG pattern\n");
        return -1;
    }
    
    printf("TPG checkerboard pattern displayed on DMD\n");
    return 0;
}

int cmd_solid(void) {
    /* Disable pattern mode first */
    if (LCR_SetMode(PTN_MODE_DISABLE) < 0) {
        printf("ERROR: Cannot disable pattern mode\n");
        return -1;
    }
    
    /* Set input source to internal test pattern */
    if (LCR_SetInputSource(1, 0) < 0) {
        printf("ERROR: Cannot set input source\n");
        return -1;
    }
    
    /* Select solid field pattern */
    if (LCR_SetTPGSelect(0) < 0) {
        printf("ERROR: Cannot select solid pattern\n");
        return -1;
    }
    
    /* Set TPG color (all white = all mirrors ON) */
    if (LCR_SetTPGColor(1023, 1023, 1023, 0, 0, 0) < 0) {
        printf("ERROR: Cannot set TPG color\n");
        return -1;
    }
    
    printf("Solid white pattern displayed - all mirrors ON\n");
    return 0;
}

int main(int argc, char *argv[]) {
    DIAG_Init("diagnostic.log", 1);

    char input[256];
    int choice;
    
    printf("DLPC900 Pattern CLI\n");
    
    if (connect_device() != 0) {
        printf("\nRunning in disconnected mode (commands will fail)\n");
    }
    
    while (1) {
        print_menu();
        printf("Enter choice: ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }
        
        choice = atoi(input);
        
        switch (choice) {
            case 1:
                cmd_status();
                break;
            case 2:
                cmd_version();
                break;
            case 3:
                cmd_mode();
                break;
            case 4:
                cmd_pattern();
                break;
            case 5:
                cmd_stop();
                break;
            case 6:
                cmd_OTF();
                break;
            case 7:
                cmd_disable();
                break;
            case 8:
                cmd_tpg();
                break;
            case 9:
                cmd_solid();
                break;
            case 0:
                printf("Stopping and exiting...\n");
                disconnect_device();
                return 0;
            default:
                printf("Invalid choice. Please enter 0-9.\n");
                break;
        }
    }
    
    disconnect_device();
    return 0;
}