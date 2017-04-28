#include "actions.h"
#include "common.h"
#include "config.h"
#include "frame_tx.h"
#include "gpio.h"
#include "handlers.h"
#include "infocenter.h"
#include "logging.h"
#include "mcu_proto.h"
#include "mem_util.h"
#include "requests.h"
#include "serial_port.h"

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

/*
 * Detected on rising edge of RESET GPIO.
 * Low = Run app from ROM
 * High = Enter download mode and wait for a new app
 */
#define SCREEN_BOOT_MODE_GPIO 7

/*
 * Resets the screen on rising edge
 */
#define SCREEN_RESET_GPIO 8

static void frame_handler(const unsigned char *frame, int len) {
    if (frame[0] != PAYLOAD_HEADER) {
        syslog(LOG_WARNING, "frame with unknown type received: %hhx\n",
               frame[0]);
        return;
    }

    extern RESPONSE_HANDLER g_response_handlers[];
    for (RESPONSE_HANDLER *handler = &g_response_handlers[0]; handler != NULL;
         handler++) {
        if (handler->type == frame[1]) {
            handler->handler(frame + 2,
                             len - 2); /* Start from payload content */
            return;
        }
    }

    syslog(LOG_WARNING, "frame with unknown response type received: %hhx\n",
           frame[1]);
}

static int screen_initialize(int skip_reset) {
    mask_memory_byte(0x1800c1c1, 0xf0, 0); /* Enable UART2 in DMU */

    if (!skip_reset) {
        if (gpio_export(SCREEN_BOOT_MODE_GPIO) == FAILURE ||
            gpio_export(SCREEN_RESET_GPIO) == FAILURE) {
            syslog(LOG_ERR, "Could not export GPIOs\n");
            return FAILURE;
        }

        if (gpio_set_direction(SCREEN_BOOT_MODE_GPIO, GPIO_OUT) == FAILURE ||
            gpio_set_direction(SCREEN_RESET_GPIO, GPIO_OUT) == FAILURE) {
            syslog(LOG_ERR, "Could not set GPIO direction\n");
            return FAILURE;
        }

        if (gpio_set_value(SCREEN_BOOT_MODE_GPIO, 0) == FAILURE ||
            gpio_set_value(SCREEN_RESET_GPIO, 0) == FAILURE ||
            gpio_set_value(SCREEN_RESET_GPIO, 1) == FAILURE) {
            syslog(LOG_ERR, "Could not reset screen\n");
            return FAILURE;
        }
    }

    if (serial_setup("/dev/ttyS1") == FAILURE) {
        syslog(LOG_ERR, "Could not setup serial transport\n");
        return FAILURE;
    }

    return request_mcu_version(); // At least we should be able to send
}

void cleanup() {
    config_free();
    syslog_stop();
}

int main(int argc, char *argv[]) {
    atexit(cleanup);

    config_load_defaults();
    config_parse_cmdline(argc, argv);

    syslog_setup(CFG->foreground);

    update_all_info();

    if (CFG->test_mode) {
        print_all_info();
        return 0;
    }

    if (screen_initialize(CFG->skip_reset) == FAILURE) {
        return -EIO;
    }

    send_initial_data();
    serial_set_pollin_callback(frame_notify_serial_recv);
    frame_set_received_callback(frame_handler);
    serial_start_poll_loop();
}
