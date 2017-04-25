#ifndef _CONFIG_H
#define _CONFIG_H

typedef struct _config {
    /**
     * This script will be called in order to get basic info
     * such as HW/SW version, MAC address, model etc.
     *
     * Expected output format (one line for each field):
     * MODEL
     * HW version
     * MAC address
     * SW version
     * 
     * Example:
     * K3
     * A1
     * 02:00:00:00:00:00
     * r3921
     */
    char* basic_info_script;
    #define DEFAULT_BASIC_INFO_SCRIPT "/lib/k3screen/basic.sh"

    /**
     * This script will be called in order to get ports info.
     *
     * Expected output format (one line for each field):
     * LAN1 connected? (0 or 1, applies to other fields as well)
     * LAN2 connected?
     * LAN3 connected?
     * WAN connected?
     * USB connected / mounted? (up to you)
     * 
     * Example:
     * 1
     * 1
     * 0
     * 1
     * 1
     */
    char* port_script;
    #define DEFAULT_PORT_SCRIPT "/lib/k3screen/port.sh"

    /**
     * This script will be called in order to get WAN speed info.
     *
     * Expected output format (one line for each field):
     * Internet connected? (0 or 1)
     * Upload speed (integer, in Bytes per sec)
     * Download speed (integer, in Bytes per sec)
     * 
     * Example:
     * 1
     * 10240000
     * 2048000
     */
    char* wan_script;
    #define DEFAULT_WAN_SCRIPT "/lib/k3screen/wan.sh"

    /**
     * This script will be called in order to get WiFi info.
     *
     * Expected output format (one line for each field):
     * Does 2.4GHz and 5GHz have same SSID? (Band steering?) (0 or 1)
     * 2.4GHz SSID
     * 2.4GHz password (or ******* if you like, applies to other fields) 
     * 2.4GHz enabled (0 or 1)
     * Number of clients connected to 2.4GHz
     * 5GHz SSID
     * 5GHz password
     * 5GHz enabled
     * Number of clients connected to 5GHz
     * Visitor network SSID
     * Visitor network password
     * Visitor network enabled
     * Number of clients connected to visitor network
     *
     * Example:
     * 0
     * LEDE-24G
     * password24
     * 1
     * 0
     * LEDE-5G
     * password5
     * 1
     * 4
     * <empty line>
     * <empty line>
     * 0
     * 0
     */
    char* wifi_script;
    #define DEFAULT_WIFI_SCRIPT "/lib/k3screen/wifi.sh"

    /**
     * This script will be called in order to get host info.
     *
     * Expected output format (one line for each field):
     * Number of hosts
     * Host1 name
     * Host1 upload speed
     * Host1 download speed
     * Host1 brand (0~29)
     * <repetition of Host1 fields>
     *
     * Example:
     * 2
     * MyHost1
     * 248193
     * 1024000
     * 25
     * MyHost2
     * 902831
     * 10485760
     * 0
     */
    char* host_script;
    #define DEFAULT_HOST_SCRIPT "/lib/k3screen/host.sh"

    /**
     * Shall we skip GPIO setup (do not reset the microcontroller)?
     * Useful when debugging.
     */
    int skip_reset;
    #define DEFAULT_SKIP_RESET 0
} CONFIG;

void config_parse_cmdline(int argc, char* argv[]);
void config_load_defaults();
CONFIG* config_get();
void config_free();

#define CFG (config_get())
#endif