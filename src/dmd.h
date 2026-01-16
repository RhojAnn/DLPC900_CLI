#ifndef CMD_H
#define CMD_H

// Connection commands (cmd_connection.c)
int is_dmd_connected(void);
int dmd_connect(void);
int dmd_disconnect(void);

// Status commands (cmd_status.c)
int dmd_status(void);
int dmd_version(void);
int dmd_toggle_idle(void);
int dmd_get_power_mode(void);
int dmd_set_standby(void);
int dmd_set_normal(void);

// Pattern commands (cmd_pattern.c)
int dmd_otf(void);
int dmd_pattern_mode(void);
int dmd_clear_pattern(void);
int dmd_disable(void);
int dmd_tpg(void);

// BMP Image loading (cmd_image.c)
int dmd_load_bmp(void);
int dmd_load_white(void);
int dmd_load_black(void);
int dmd_load_half(void);

#endif