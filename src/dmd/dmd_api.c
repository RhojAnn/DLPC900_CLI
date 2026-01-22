/**
 * dmd_api.c
 * DLL wrapper for DLPC900 DMD control functions
 * Exports functions for use with Python ctypes
 */

#include <stdio.h>
#include "dmd.h"
#include "..\lib\API.h"

#ifdef _WIN32
    #define DMD_API __declspec(dllexport)
#else
    #define DMD_API
#endif

// ============== Connection ==============

DMD_API int dmd_is_connected(void) {
    return cmd_is_connected();
}

DMD_API int dmd_connect(void) {
    return cmd_connect();
}

DMD_API int dmd_disconnect(void) {
    return cmd_disconnect();
}

// ============== Status ==============

DMD_API int dmd_get_status(unsigned char* hw, unsigned char* sys, 
                           unsigned char* main_status, unsigned char* dlpa, 
                           unsigned char* dmd) {
    if (!hw || !sys || !main_status || !dlpa || !dmd) return -1;
    return LCR_GetStatus(hw, sys, main_status, dlpa, dmd);
}

DMD_API int dmd_get_version(unsigned int* app, unsigned int* api, 
                            unsigned int* swconfig, unsigned int* seqconfig) {
    if (!app || !api || !swconfig || !seqconfig) return -1;
    return LCR_GetVersion(app, api, swconfig, seqconfig);
}

DMD_API int dmd_get_power_mode(unsigned int* is_on_standby) {
    if (!is_on_standby) return -1;
    int res = cmd_get_power_mode();
    if (res < 0) return -1;
    *is_on_standby = (unsigned int)res;
    return 0;
}

DMD_API int dmd_set_standby(void) {
    return cmd_set_standby();
}

DMD_API int dmd_set_normal(void) {
    return cmd_set_normal();
}

DMD_API int dmd_toggle_idle(void) {
    return cmd_toggle_idle();
}

DMD_API int dmd_software_reset(void) {
    return cmd_software_reset();
}

// ============== Pattern Mode ==============

DMD_API int dmd_set_otf_mode(void) {
    return cmd_otf();
}

DMD_API int dmd_set_disable_mode(void) {
    return cmd_disable();
}

DMD_API int dmd_get_pattern_mode(int* mode) {
    if (!mode) return -1;
    API_DisplayMode_t m;
    if (LCR_GetMode(&m) < 0) return -1;
    *mode = (int)m;
    return 0;
}

DMD_API int dmd_clear_pattern(void) {
    return cmd_clear_pattern();
}

DMD_API int dmd_show_tpg(void) {
    return cmd_tpg();
}

// ============== LED Control ==============

DMD_API int dmd_set_led_enables(int seq_ctrl, int red, int green, int blue) {
    return LCR_SetLedEnables(seq_ctrl, red, green, blue);
}

DMD_API int dmd_get_led_enables(int* seq_ctrl, int* red, int* green, int* blue) {
    if (!seq_ctrl || !red || !green || !blue) return -1;
    int s, r, g, b;
    if (LCR_GetLedEnables(&s, &r, &g, &b) < 0) return -1;
    *seq_ctrl = s;
    *red = r;
    *green = g;
    *blue = b;
    return 0;
}

// ============== Image Display ==============

DMD_API int dmd_display_bmp(const char* filename) {
    return cmd_display_bmp(filename);
}

DMD_API int dmd_load_white(void) {
    return cmd_load_white();
}

DMD_API int dmd_load_black(void) {
    return cmd_load_black();
}

DMD_API int dmd_load_half(void) {
    return cmd_load_half();
}
