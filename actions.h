#ifndef _ACTIONS_H
#define _ACTIONS_H

#include "mcu_proto.h"

int select_prev_host_page();
int select_next_host_page();
void send_page_data(PAGE page);
void send_initial_data();

#endif
