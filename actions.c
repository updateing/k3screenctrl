#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "common.h"
#include "infocenter.h"
#include "mcu_proto.h"
#include "requests.h"

static int g_host_page = 0;

struct _host_info_single *get_hosts() {
    return g_host_info_array;
}

int get_hosts_count() { return g_host_info_elements; }

int select_prev_host_page() {
    if (g_host_page <= 0) {
        return FAILURE;
    }

    g_host_page--;
    return SUCCESS;
}

int select_next_host_page() {
    if (get_hosts_count() - g_host_page * (HOSTS_PER_PAGE + 1) <= 0) {
        return FAILURE;
    }

    g_host_page++;
    return SUCCESS;
}

void send_page_data(PAGE page) {
    switch (page) {
    case PAGE_BASIC_INFO:
        request_update_basic_info(
            g_basic_info.product_name, g_basic_info.hw_version,
            g_basic_info.fw_version, g_basic_info.mac_addr_base);
        break;
    case PAGE_PORTS:
        request_update_ports(&g_port_info);
        break;
    case PAGE_WAN:
        request_update_wan(g_wan_info.is_connected, g_wan_info.tx_bytes_per_sec,
                           g_wan_info.rx_bytes_per_sec);
        break;
    case PAGE_WIFI:
        request_update_wifi(&g_wifi_info);
        break;
    case PAGE_HOSTS:
        request_update_hosts_paged(get_hosts(), get_hosts_count(),
                                   g_host_page * HOSTS_PER_PAGE);
        break;
    default:
        syslog(LOG_WARNING, "unknown page requested: %d\n", page);
        break;
    }
}

void send_initial_data() {
    send_page_data(PAGE_BASIC_INFO);
    send_page_data(PAGE_PORTS);
    send_page_data(PAGE_WAN);
    send_page_data(PAGE_WIFI);
    send_page_data(PAGE_HOSTS);
    request_switch_page(PAGE_WAN);
}