#include <string.h>

#include "serial_port.h"
#include "mcu_proto.h"
#include "frame_tx.h"

int request_notify_status(STATUS_NOTIF status) {
    unsigned char cmdbuf[6];
    bzero(cmdbuf, sizeof(cmdbuf));
    cmdbuf[0] = PAYLOAD_HEADER;
    cmdbuf[1] = REQUEST_NOTIFY_STATUS;
    cmdbuf[2] = status;
    return frame_send(cmdbuf, sizeof(cmdbuf));
}

int request_update_wan(int is_connected, int tx_bps, int rx_bps) {
    unsigned char cmdbuf[2 + sizeof(WAN_INFO)];
    WAN_INFO waninfo;

    cmdbuf[0] = PAYLOAD_HEADER;
    cmdbuf[1] = REQUEST_UPDATE_WAN;

    waninfo.is_connected = is_connected;
    waninfo.tx_bytes_per_sec = tx_bps;
    waninfo.rx_bytes_per_sec = rx_bps;

    memmove(cmdbuf + 2, &waninfo, sizeof(waninfo));
    return frame_send(cmdbuf, sizeof(cmdbuf));
}
