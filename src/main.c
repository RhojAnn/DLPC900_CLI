/*
* DLPC900 Pattern CLI
* Simple command line interface to control DLPC900 pattern modes
* Will be extended with a python wrapper for GUI use   
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"
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
    printf(" 10. Load BMP image to DMD\n");
    printf("  0. Exit\n");
    printf("\n");
}

int main(int argc, char *argv[]) {
    DIAG_Init("diagnostic.log", 0);

    char input[256];
    int choice;
    
    printf("DLPC900 Pattern CLI\n");
    
    if (connect_device() != 0) 
        printf("\nRunning in disconnected mode (commands will fail)\n");
    
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
            case 1:  cmd_status();    break;
            case 2:  cmd_version();   break;
            case 3:  cmd_mode();      break;
            case 4:  cmd_pattern();   break;
            case 5:  cmd_stop();      break;
            case 6:  cmd_otf();       break;
            case 7:  cmd_disable();   break;
            case 8:  cmd_tpg();       break;
            case 9:  cmd_solid();     break;
            case 10: cmd_load_bmp();  break;
            case 0:
                printf("Stopping and exiting...\n");
                disconnect_device();
                return 0;
            default:
                printf("Invalid choice. Please enter 0-10.\n");
                break;
        }
    }
    
    disconnect_device();
    return 0;
}