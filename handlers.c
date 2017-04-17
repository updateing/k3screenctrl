#include <syslog.h>
#include <stdio.h>
#include <string.h>

#include "mcu_proto.h"
#include "handlers.h"
#include "requests.h"

static MCU_VERSION g_mcu_version;
void handle_mcu_version(const unsigned char* payload, int len) {
    if (len < 4) {
        syslog(LOG_WARNING, "Got malformed MCU version response. Length is %d\n", len);
        return;
    }
    g_mcu_version.patch_ver = payload[0] | payload[1] << 8; /* Do we need this endian compatabitity? */
    g_mcu_version.minor_ver = payload[2];
    g_mcu_version.major_ver = payload[3];

    syslog(LOG_INFO, "MCU reported version as %hhd.%hhd.%hd\n", g_mcu_version.major_ver,
           g_mcu_version.minor_ver, g_mcu_version.patch_ver);
}

static int g_current_page = 3, g_is_screen_on = 1;
static void send_page_data();
void handle_key_press(const unsigned char* payload, int len) {
    if (len < 1) {
        syslog(LOG_WARNING, "Got malformed key press response. Length is %d\n", len);
        return;
    }
    if (!g_is_screen_on) {
        request_notify_status(STATUS_WAKEUP);
        g_is_screen_on = 1;
        return;
    }
    switch (payload[0]) {
        case KEY_LEFT_SHORT:
            if (g_current_page > 1) {
                g_current_page--;
            }
            printf("KEY_LEFT_SHORT\n");
            break;
        case KEY_RIGHT_SHORT:
            if (g_current_page < 5) {
                g_current_page++;
            }
            printf("KEY_RIGHT_SHORT\n");
            break;
        case KEY_MIDDLE_SHORT:
            g_current_page = 3;
            printf("KEY_MIDDLE_SHORT\n");
            break;
        case KEY_MIDDLE_LONG:
            request_notify_status(STATUS_SLEEP);
            g_is_screen_on = 0;
            return;
        case KEY_LEFT_LONG:
            // Implement something fun
            request_update_wan(1, 233000000, 666000000);
            printf("KEY_LEFT_LONG\n");
            return;
        case KEY_RIGHT_LONG:
            request_update_wan(1, 666000000, 999000000);
            printf("KEY_RIGHT_LONG\n");
            return;
        default:
            syslog(LOG_WARNING, "unknown key code: %hhx\n", payload[0]);
            return;
    }

    send_page_data();
    request_switch_page(g_current_page);
    printf("current page = %d\n", g_current_page);
}

static void send_page_data() {
    switch (g_current_page) {
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

                strcpy(wifi_info.wl_5g_info.ssid, "LEDE-K3-5G");
                strcpy(wifi_info.wl_5g_info.psk, "12345678");
                wifi_info.wl_5g_info.enabled = 1;

                strcpy(wifi_info.wl_visitor_info.ssid, "LEDE-K3-Visitor");
                strcpy(wifi_info.wl_visitor_info.psk, "12345678");
                wifi_info.wl_visitor_info.enabled = 1;
                request_update_wifi(&wifi_info);
            } while (0);
            break;
        case 5:
            // request_update_hosts
            break;
        default:
            // ?
            break;
    }
}

RESPONSE_HANDLER g_response_handlers[] = {
    {RESPONSE_MCU_VERSION, handle_mcu_version},
    {RESPONSE_KEY_PRESS, handle_key_press},
};
