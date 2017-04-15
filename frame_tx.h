#ifndef _FRAME_TX_H
#define _FRAME_TX_H

int frame_send(const unsigned char* data, int len);
void serial_recv_callback();
void frame_set_received_callback(void (*func)(const unsigned char*, int));

#endif
