#include <string.h>
#include <syslog.h>
#include <stdio.h>

#include "mcu_proto.h"
#include "requests.h"
#include "common.h"

static int g_host_page = 0;

/***************** TEST BEGIN *********************/
struct _host_info_single hosts[] = {
    {23300, 66600, "H", 0},
    {66600, 99900, "H2", 1},
    {66600, 99900, "H3", 2},
    {23300, 66600, "H4", 3},
    {66600, 99900, "H5", 4},
    {23300, 66600, "H6", 5},
    {66600, 99900, "H7", 6},
    {66600, 99900, "H8", 7},
    {23300, 66600, "H9", 8},
    {66600, 99900, "10", 9},
    {23300, 66600, "11", 10},
    {66600, 99900, "12", 11},
    {66600, 99900, "13", 12},
    {23300, 66600, "14", 13},
    {66600, 99900, "15", 14},
    {23300, 66600, "16", 15},
    {66600, 99900, "17", 16},
    {66600, 99900, "18", 17},
    {23300, 66600, "19", 18},
    {66600, 99900, "20", 19},
    {66600, 99900, "21", 20},
    {23300, 66600, "22", 21},
    {66600, 99900, "23", 22},
    {23300, 66600, "24", 23},
    {66600, 99900, "25", 24},
    {23300, 66600, "26", 25},
    {66600, 99900, "27", 26},
    {66600, 99900, "28", 27},
    {23300, 66600, "29", 28},
    {66600, 99900, "30", 29}
};

struct _host_info_single* get_hosts() {
    return hosts;
}

int get_hosts_count() {
    return sizeof(hosts)/sizeof(hosts[0]);
}

/***************** TEST END *********************/

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
            request_update_basic_info("K3-LEDE", "A-Any", "r3921+6", "23:33:66:66:99:99");
            break;
        case PAGE_PORTS:
            do {
                PORT_INFO port_info;
                port_info.eth_port1_conn = 1;
                port_info.eth_port2_conn = 1;
                port_info.eth_port3_conn = 1;
                port_info.eth_wan_conn = 0;
                port_info.usb_conn = 1;
                request_update_ports(&port_info);
            } while(0);
            break;
        case PAGE_WAN:
            request_update_wan(1, 233000000, 666000000);
            break;
        case PAGE_WIFI:
            do {
                WIFI_INFO wifi_info;
                wifi_info.band_mix = 0;

                strcpy(wifi_info.wl_24g_info.ssid, "LEDE-K3");
                strcpy(wifi_info.wl_24g_info.psk, "12345678");
                wifi_info.wl_24g_info.enabled = 1;
                wifi_info.wl_24g_info.clients_count = 11;

                strcpy(wifi_info.wl_5g_info.ssid, "LEDE-K3-5G");
                strcpy(wifi_info.wl_5g_info.psk, "12345678");
                wifi_info.wl_5g_info.enabled = 1;
                wifi_info.wl_5g_info.clients_count = 87;

                strcpy(wifi_info.wl_visitor_info.ssid, "LEDE-K3-Visitor");
                strcpy(wifi_info.wl_visitor_info.psk, "12345678");
                wifi_info.wl_visitor_info.enabled = 1;
                wifi_info.wl_visitor_info.clients_count = 20;
                request_update_wifi(&wifi_info);
            } while (0);
            break;
        case PAGE_HOSTS:
            request_update_hosts_paged(get_hosts(), get_hosts_count(), g_host_page * HOSTS_PER_PAGE);
            break;
        default:
            syslog(LOG_WARNING, "unknown page requested: %hhd\n", page);
            break;
    }
}
