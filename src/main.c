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
    printf("\nStatus Commands\n");
    printf("  1. Get device status\n");
    printf("  2. Get firmware version\n");
    printf("\nPower Commands\n");
    printf("  3. Toggle idle mode (Recommended when DMD is not in use)\n");
    printf(" 13. Set standby mode\n");
    printf(" 14. Set normal mode\n");
    printf(" 15. Show power mode\n");
    printf("\nPattern Commands\n");
    printf("  4. Get current pattern mode\n");
    printf("  5. Switch to OTF mode\n");
    printf("  6. Disable pattern mode\n");
    printf("  7. Stop pattern display\n");
    printf("  8. Show TPG test pattern (checkerboard)\n");
    printf("\nImage Loading Commands\n");
    printf("  9. Load BMP image to DMD\n");
    printf(" 10. Load half-tone BMP image to DMD\n");
    printf(" 11. Load white BMP image to DMD\n");
    printf(" 12. Load black BMP image to DMD\n");
    printf("  0. Exit\n");
    printf("\n");
}

int main(int argc, char *argv[]) {
    // DIAG_Init("diagnostic.log", 0);

    char input[256];
    int choice;
    
    printf("DLPC900 Pattern CLI\n");
    


    if (dmd_connect() != 0) 
        printf("\nRunning in disconnected mode (commands will fail)\n");
    

    dmd_set_normal();

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
            case 1:  dmd_status();          break;
            case 2:  dmd_version();         break;
            case 3:  dmd_toggle_idle();     break;
            case 4:  dmd_pattern_mode();    break;
            case 5:  dmd_otf();             break;
            case 6:  dmd_disable();         break;
            case 7:  dmd_clear_pattern();   break;
            case 8:  dmd_tpg();             break;
            case 9:  dmd_load_bmp();        break;
            case 10: 
                dmd_clear_pattern();   
                dmd_load_half();
                break;
            case 11: 
                dmd_clear_pattern();   
                dmd_load_white();
                break;
            case 12: 
                dmd_clear_pattern();   
                dmd_load_black();
                break;
            case 13:
                dmd_set_standby();
                break;
            case 14:
                dmd_set_normal();
                break;
            case 15:
                dmd_get_power_mode();
                break;
            case 0:
                printf("Stopping and exiting...\n");
                // cmd_set_standby();
                dmd_disconnect();
                return 0;
            default:
                printf("Invalid choice. Please enter 0-15.\n");
                break;
        }
    }
    
    dmd_disconnect();
    return 0;
}