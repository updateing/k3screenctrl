#include <syslog.h>
#include <stdio.h>

#include "mcu_proto.h"
#include "handlers.h"

static MCU_VERSION g_mcu_version;
void handle_mcu_version(unsigned char* payload, int len) {
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

static int g_current_page = 3;
void handle_key_press(unsigned char* payload, int len) {
    if (len < 1) {
        syslog(LOG_WARNING, "Got malformed key press response. Length is %d\n", len);
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
            // TODO turn off
            printf("KEY_MIDDLE_LONG\n");
            return;
        case KEY_LEFT_LONG:
            // Implement something fun
            printf("KEY_LEFT_LONG\n");
            return;
        case KEY_RIGHT_LONG:
            printf("KEY_RIGHT_LONG\n");
            return;
        default:
            syslog(LOG_WARNING, "unknown key code: %hhx\n", payload[0]);
            return;
    }

    // send_page_data();
    printf("current page = %d\n", g_current_page);
}

RESPONSE_HANDLER g_response_handlers[] = {
    {RESPONSE_MCU_VERSION, handle_mcu_version},
    {RESPONSE_KEY_PRESS, handle_key_press},
};
