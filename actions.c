#include <string.h>

#include "mcu_proto.h"
#include "requests.h"

void send_page_data(PAGE page) {
    switch (page) {
        case 1:
            request_update_basic_info("K3-LEDE", "A-Any", "r3921+6", "23:33:66:66:99:99");
            break;
        case 2:
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
        case 3:
            request_update_wan(1, 233000000, 666000000);
            break;
        case 4:
            // request_update_wifi
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
        case 5:
            // request_update_hosts
            do {
                struct _host_info_single hosts[] = {
                    {23300, 66600, "H", 0},
                    {66600, 99900, "H2", 1},
                    {66600, 99900, "H3", 2},
                    {23300, 66600, "H4", 3},
                    {66600, 99900, "H5", 4},
                };
                request_update_hosts_paged(hosts, sizeof(hosts) / sizeof(hosts[0]), 0);
            } while (0);
            break;
        default:
            // ?
            break;
    }
}
