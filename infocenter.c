#include <errno.h>
#include <string.h>
#include <syslog.h>

#include "common.h"
#include "config.h"
#include "mcu_proto.h"
#include "scripts.h"

enum _token_type {
    TOKEN_STRING_NEW,       /* Duplicate a new string and write its pointer to
                               *storage */
    TOKEN_STRING_OVERWRITE, /* Write the result directly to *storage, taking
                               storage_len into consideration */
    TOKEN_INT               /* Write the result to *storage */
};

struct _token_store {
    void **storage;
    enum _token_type type;
    int storage_len;
};

/* Use the value rather than its address for x */
#define TOKEN_STRING_OVERWRITE_STORE(x)                                        \
    { &(x), TOKEN_STRING_OVERWRITE, sizeof((x)) }
#define TOKEN_STRING_NEW_STORE(x)                                              \
    { &(x), TOKEN_STRING_NEW, 0 }
#define TOKEN_INT_STORE(x)                                                     \
    { &(x), TOKEN_INT, 0 }

/* Returns pointer to where it left */
static char *tokenize_and_store(const char *str, const char delim,
                                const struct _token_store *stores,
                                int store_len) {
    char *last_start = str;
    char *next_token;
    int token_pos = 0;

    while (token_pos < store_len && (next_token = strchr(last_start, delim))) {
        char *curr_token = strndup(last_start, next_token - last_start);
        switch (stores[token_pos].type) {
        case TOKEN_STRING_NEW:
            *stores[token_pos].storage = curr_token;
            break;
        case TOKEN_STRING_OVERWRITE:
            strncpy(*stores[token_pos].storage, curr_token,
                    stores[token_pos].storage_len);
            break;
        case TOKEN_INT:
            *stores[token_pos].storage = atoi(curr_token);
            free(curr_token);
            break;
        }
        last_start = next_token + 1;
        token_pos++;
    }

    if (token_pos < store_len && *last_start != 0) {
        /* We want more tokens, and there is the case where we might
         * miss a token:
         * The input is not delimiter-terminated, the last token was not
         * processed. */
        char *curr_token = strdup(last_start);
        last_start += strlen(curr_token);
        switch (stores[token_pos].type) {
        case TOKEN_STRING_NEW:
            *stores[token_pos].storage = curr_token;
            break;
        case TOKEN_STRING_OVERWRITE:
            strncpy(*stores[token_pos].storage, curr_token,
                    stores[token_pos].storage_len);
            free(curr_token);
            break;
        case TOKEN_INT:
            *stores[token_pos].storage = atoi(curr_token);
            free(curr_token);
            break;
        }
        token_pos++;
    }

    if (token_pos < store_len) {
        /* Still want more tokens */
        syslog(LOG_WARNING, "tokenizer: found %d tokens but expected %d (%s)\n",
               token_pos, store_len, str);
    }
    return last_start;
}

static int update_storage_from_script(const char *script,
                                      const struct _token_store stores[],
                                      int store_len) {
    char *out = script_get_output(script);
    if (out == NULL) {
        return FAILURE;
    }

    /* If the tokenizer stopped at \0, the entire output should have been
     * procesed successfully.
     * If it did not (there is something left in the output), the output may
     * be malformatted and the results are not reliable.
     */
    return *tokenize_and_store(out, '\n', stores,
                               sizeof(stores) / sizeof(stores[0])) == 0
               ? SUCCESS
               : FAILURE;
}

static BASIC_INFO g_basic_info;
int update_basic_info() {
    static const struct _token_store stores[] = {
        TOKEN_STRING_OVERWRITE_STORE(g_basic_info.product_name),
        TOKEN_STRING_OVERWRITE_STORE(g_basic_info.hw_version),
        TOKEN_STRING_OVERWRITE_STORE(g_basic_info.mac_addr_base),
        TOKEN_STRING_OVERWRITE_STORE(g_basic_info.fw_version),
    };
    return update_storage_from_script(CFG->basic_info_script, stores,
                                      sizeof(stores) / sizeof(stores[0]));
}

static PORT_INFO g_port_info;
int update_port_info() {
    static const struct _token_store stores[] = {
        TOKEN_INT_STORE(g_port_info.eth_port1_conn),
        TOKEN_INT_STORE(g_port_info.eth_port2_conn),
        TOKEN_INT_STORE(g_port_info.eth_port3_conn),
        TOKEN_INT_STORE(g_port_info.eth_wan_conn),
        TOKEN_INT_STORE(g_port_info.usb_conn),
    };
    return update_storage_from_script(CFG->port_script, stores,
                                      sizeof(stores) / sizeof(stores[0]));
}

static WAN_INFO g_wan_info;
int update_wan_info() {
    static const struct _token_store stores[] = {
        TOKEN_INT_STORE(g_wan_info.is_connected),
        TOKEN_INT_STORE(g_wan_info.tx_bytes_per_sec),
        TOKEN_INT_STORE(g_wan_info.rx_bytes_per_sec),
    };
    return update_storage_from_script(CFG->wan_script, stores,
                                      sizeof(stores) / sizeof(stores[0]));
}

static WIFI_INFO g_wifi_info;
int update_wifi_info() {
    static const struct _token_store stores[] = {
        TOKEN_INT_STORE(g_wifi_info.band_mix),

        TOKEN_STRING_OVERWRITE_STORE(g_wifi_info.wl_24g_info.ssid),
        TOKEN_STRING_OVERWRITE_STORE(g_wifi_info.wl_24g_info.psk),
        TOKEN_INT_STORE(g_wifi_info.wl_24g_info.enabled),
        TOKEN_INT_STORE(g_wifi_info.wl_24g_info.sta_count),

        TOKEN_STRING_OVERWRITE_STORE(g_wifi_info.wl_5g_info.ssid),
        TOKEN_STRING_OVERWRITE_STORE(g_wifi_info.wl_5g_info.psk),
        TOKEN_INT_STORE(g_wifi_info.wl_5g_info.enabled),
        TOKEN_INT_STORE(g_wifi_info.wl_5g_info.sta_count),

        TOKEN_STRING_OVERWRITE_STORE(g_wifi_info.wl_visitor_info.ssid),
        TOKEN_STRING_OVERWRITE_STORE(g_wifi_info.wl_visitor_info.psk),
        TOKEN_INT_STORE(g_wifi_info.wl_visitor_info.enabled),
        TOKEN_INT_STORE(g_wifi_info.wl_visitor_info.sta_count),
    };
    return update_storage_from_script(CFG->wifi_script, stores,
                                      sizeof(stores) / sizeof(stores[0]));
}

static struct _host_info_single *g_host_info_array;
static int g_host_info_elements;
int update_host_info() {
    int ret = FAILURE;
    char *out = script_get_output(CFG->host_script);
    char *curr_pos = out;
    if (out == NULL) {
        goto final_exit;
    }

    static const struct _token_store number_token[] = {
        TOKEN_INT_STORE(g_host_info_elements),
    };
    curr_pos = tokenize_and_store(out, '\n', number_token, 1);

    /* If there is no hosts, we won't bother to consider what's next */
    if (g_host_info_elements == 0) {
        ret = SUCCESS;
        goto free_exit;
    }

    if (g_host_info_array != NULL) {
        free(g_host_info_array);
    }

    g_host_info_array = (struct _host_info_single *)malloc(
        g_host_info_elements * sizeof(struct _host_info_single));
    if (g_host_info_array == NULL) {
        syslog(LOG_ERR, "could not allocate memory for host info: %s\n",
               strerror(errno));
        goto free_exit;
    }

    /* The storage pointer is just placeholders. We will change later */
    struct _token_store host_info_tokens[] = {
        TOKEN_INT_STORE(g_host_info_array[0].download_Bps),
        TOKEN_INT_STORE(g_host_info_array[0].upload_Bps),
        TOKEN_STRING_OVERWRITE_STORE(g_host_info_array[0].hostname),
        TOKEN_INT_STORE(g_host_info_array[0].logo),
    };
    for (int i = 0; i < g_host_info_elements; i++) {
        host_info_tokens[0].storage = &g_host_info_array[i].download_Bps;
        host_info_tokens[1].storage = &g_host_info_array[i].upload_Bps;
        host_info_tokens[2].storage = &g_host_info_array[i].hostname;
        host_info_tokens[3].storage = &g_host_info_array[i].logo;

        curr_pos = tokenize_and_store(curr_pos, '\n', host_info_tokens,
                                      sizeof(host_info_tokens) /
                                          sizeof(host_info_tokens[0]));

        if (*curr_pos == 0) {
            syslog(LOG_ERR, "output from host info script was incomplete. "
                            "Informed with %d hosts but only read %d hosts."
                            "Showing read hosts only\n",
                   g_host_info_elements, i);
            g_host_info_elements = i;
            goto free_exit;
        }
    }

    ret = SUCCESS;
free_exit:
    free(out);
final_exit:
    return ret;
}