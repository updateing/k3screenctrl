#include <getopt.h>

#include "config.h"

static CONFIG g_config;

static void config_show_help() {
    fprintf(stderr,
           "USAGE: k3screenctrl [OPTIONS]\n\n"
           "\t\t\t-h, --help\t\tShow this help"
           "\t\t\t-r, --skip-reset\t\tDo not reset screen on startup (will reset by default)\n"
           "\t\t\t-s, --host-script <PATH>\t\tUse this script to gather hosts info\n"
           "\t\t\t-w, --wifi-script <PATH>\t\tUse this script to gather WiFi info\n"
           "\t\t\t-w, --port-script <PATH>\t\tUse this script to gather port info\n"
           "\t\t\t-w, --wan-script <PATH>\t\tUse this script to gather WAN speed and internet connection info\n"
           "\t\t\t-w, --basic-info-script <PATH>\t\tUse this script to gather basic info\n"
           "\nThe defaults are /lib/k3screenctrl/{host,wifi,port,wan,basic}.sh\n");
    exit(1);
}

void config_parse_cmdline(int argc, char* argv[]) {
    static const struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"skip-reset", no_argument, NULL, 'r'},
        {"host-script", required_argument, NULL, 's'},
        {"wifi-script", required_argument, NULL, 'w'},
        {"port-script", required_argument, NULL, 'p'},
        {"wan-script", required_argument, NULL, 'n'},
        {"basic-info-script", required_argument, NULL, 'i'},
        {0, 0, 0, 0}
    };
    static const char* short_opts = "hrs:w:p:n:i:";

    int opt_index;
    char result;
    while ((result = getopt_long(argc, argv, short_opts, long_options, &opt_index)) != -1) {
        switch (result) {
        case 'h':
            config_show_help();
            break;
        case 'r':
            g_config.skip_reset = 1;
            break;
        case 's':
            free(g_config.host_script);
            g_config.host_script = strdup(optarg);
            break;
        case 'w':
            free(g_config.wifi_script);
            g_config.wifi_script = strdup(optarg);
            break;
        case 'p':
            free(g_config.port_script);
            g_config.port_script = strdup(optarg);
            break;
        case 'n':
            free(g_config.wan_script);
            g_config.wan_script = strdup(optarg);
            break;
        case 'i':
            free(g_config.basic_info_script);
            g_config.basic_info_script = strdup(optarg);
            break;
        }
    }
}

void config_load_defaults() {
    g_config.skip_reset = DEFAULT_SKIP_RESET;
    g_config.host_script = strdup(DEFAULT_HOST_SCRIPT);
    g_config.wifi_script = strdup(DEFAULT_WIFI_SCRIPT);
    g_config.port_script = strdup(DEFAULT_PORT_SCRIPT);
    g_config.wan_script = strdup(DEFAULT_WAN_SCRIPT);
    g_config.basic_info_script = strdup(DEFAULT_BASIC_INFO_SCRIPT);
}

void config_free() {
    free(g_config.host_script);
    free(g_config.wifi_script);
    free(g_config.port_script);
    free(g_config.wan_script);
    free(g_config.basic_info_script);
}

CONFIG* config_get() {
    return g_config;
}