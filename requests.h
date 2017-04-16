#ifndef _REQUESTS_H
#define _REQUESTS_H

#include "mcu_proto.h"

int request_switch_page(int page);
int request_notify_status(STATUS_NOTIF status);
int request_update_wan(int is_connected, int tx_bps, int rx_bps);
int request_update_basic_info(const char* prod_name, const char* hw_ver, const char* fw_ver, const char* mac_addr);
int request_update_ports(PORT_INFO *port_info);
#endif
