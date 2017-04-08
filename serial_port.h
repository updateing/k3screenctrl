#ifndef _SERIAL_PORT_H
#define _SERIAL_PORT_H

int serial_setup(const char* dev_path);
void serial_close();
int serial_write(const unsigned char* data, int len);

#endif
