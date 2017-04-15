#ifndef _REQUESTS_H
#define _REQUESTS_H

#include "mcu_proto.h"

int request_notify_status(STATUS_NOTIF status);
int request_update_wan(int is_connected, int tx_bps, int rx_bps);
#endif
