#ifndef _MCU_PROTO_H
#define _MCU_PROTO_H

/*
 * Frame structure:
 *
 * FRAME_HEADER | PAYLOAD_HEADER | PAYLOAD_TYPE | PAYLOAD | FRAME_TRAILER
 *
 * PAYLOAD_TYPE can be one of the item in REQUEST_TYPE (Tx) or RESPONSE_TYPE (Rx)
 * 0x01, 0x04 and 0x10 in PAYLOAD_* needs to be escaped by FRAME_ESCAPE.
 *
 * Actually there are frames with 0x01 and 0x02 as PAYLOAD_HEADER. Maybe this kind of
 * frames is only used when communicating with bootloader. We do not need them here
 * since we are not upgrading the bootloader.
 */

#define FRAME_HEADER 0x01
#define FRAME_TRAILER 0x04
#define FRAME_ESCAPE 0x10
#define PAYLOAD_HEADER 0x30

typedef enum _request_type {
    REQUEST_GET_MCU_VERSION = 1,
    REQUEST_UPDATE_LAN_USB = 5,
    REQUEST_UPDATE_WAN,
    REQUEST_UPDATE_WIFI,
    REQUEST_UPDATE_HOSTS,
    REQUEST_UPDATE_BASIC_INFO,
    REQUEST_NOTIFY_STATUS
} REQUEST_TYPE;

/* REQUEST_NOTIFY_STATUS */
typedef enum _status_notif {
    STATUS_WIFI_ON = 0,
    STATUS_WIFI_OFF,
    STATUS_24G_ON,
    STATUS_24G_OFF,
    STATUS_5G_ON,
    STATUS_5G_OFF,
    STATUS_WISP_ON,
    STATUS_WISP_OFF,
    STATUS_WAKEUP = 9,
    STATUS_SLEEP,
    STATUS_RESTORE_BACKUP,
    STATUS_UPGRADE,
    STATUS_REBOOT,
    STATUS_RESET,
    STATUS_WIFI_ON_2, /* Not known */
} STATUS_NOTIF;

typedef enum _response_type {
    RESPONSE_MCU_VERSION = 1,
    RESPONSE_KEY_PRESS = 3
} RESPONSE_TYPE;

/* RESPONSE_MCU_VERSION */
typedef struct _mcu_version {
    unsigned short patch_ver;
    unsigned char minor_ver;
    unsigned char major_ver;
} MCU_VERSION;

/* RESPONSE_KEY_PRESS */
typedef enum _key_code {
    KEY_LEFT_SHORT = 1,
    KEY_LEFT_LONG = 3,
    KEY_RIGHT_SHORT = 4,
    KEY_RIGHT_LONG = 6,
    KEY_MIDDLE_SHORT = 7,
    KEY_MIDDLE_LONG = 9
} KEY_CODE;

typedef struct _wan_info {
    unsigned int is_connected;
    unsigned int tx_bytes_per_sec;
    unsigned int rx_bytes_per_sec;
} WAN_INFO;
#endif
