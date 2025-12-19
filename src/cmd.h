#ifndef CMD_H
#define CMD_H

// Connection commands (cmd_connection.c)
int connect_device(void);
void disconnect_device(void);

// Status commands (cmd_status.c)
int cmd_status(void);
int cmd_version(void);
int cmd_toggle_idle(void);

// Pattern commands (cmd_pattern.c)
int cmd_pattern(void);
int cmd_otf(void);
int cmd_pattern_mode(void);
int cmd_clear_pattern(void);
int cmd_disable(void);
int cmd_tpg(void);
int cmd_solid(void);

// BMP Image loading (cmd_image.c)
int cmd_load_bmp(void);

#endif