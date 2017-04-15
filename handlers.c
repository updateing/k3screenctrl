#include <syslog.h>

static short g_mcu_ver_major, g_mcu_ver_minor, g_mcu_ver_patch;

void handle_mcu_version(unsigned char* frame, int len) {
    g_mcu_ver_patch = frame[3] | frame[4] << 8;
    g_mcu_ver_minor = frame[5];
    g_mcu_ver_major = frame[6];

    syslog(LOG_DEBUG, "MCU reported version as %hd.%hd.%hd\n", g_mcu_ver_major, g_mcu_ver_minor, g_mcu_ver_patch);
}
