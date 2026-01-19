#ifndef CMD_H
#define CMD_H

// Connection commands (dmd_connection.c)
int cmd_is_connected(void);
int cmd_connect(void);
int cmd_disconnect(void);

// Status commands (dmd_status.c)
int cmd_status(void);
int cmd_version(void);
int cmd_toggle_idle(void);
int cmd_get_power_mode(void);
int cmd_set_standby(void);
int cmd_set_normal(void);

// Pattern commands (dmd_pattern.c)
int cmd_otf(void);
int cmd_pattern_mode(void);
int cmd_clear_pattern(void);
int cmd_disable(void);
int cmd_tpg(void);

// BMP Image loading (dmd_image.c)
int cmd_display_bmp(const char *filename);
int cmd_load_bmp(void);
int cmd_load_white(void);
int cmd_load_black(void);
int cmd_load_half(void);

#endif