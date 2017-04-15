#ifndef _SERIAL_PORT_H
#define _SERIAL_PORT_H

#define SERIAL_POLL_INTERVAL_MS 500

int serial_setup(const char* dev_path);
void serial_close();
int serial_write(const unsigned char* data, int len);
int serial_read(unsigned char* buf, int maxlen);
void serial_set_pollin_callback(void (*callback)());
void serial_break_poll_loop();
void serial_start_poll_loop();

#endif
