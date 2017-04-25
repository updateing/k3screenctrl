#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "common.h"
#include "logging.h"
#include "serial_port.h"

int g_serial_fd = -1;
int g_poll_loop_break_flag = 0;
void (*g_serial_pollin_callback)();

int serial_setup(const char *dev_path) {
    struct termios serial_termios;

    g_serial_fd = open(dev_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (g_serial_fd == -1) {
        syslog(LOG_CRIT, "could not open %s\n", dev_path);
        return FAILURE;
    }

    if (tcgetattr(g_serial_fd, &serial_termios)) {
        syslog(LOG_ERR, "could not get attrs from serial fd: %s\n",
               strerror(errno));
        serial_close();
        return FAILURE;
    }

    /* 115200 */
    cfsetspeed(&serial_termios, B115200);

    /* Noncanonical mode, no echo */
    serial_termios.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE);

    /* No XON/XOFF flow control */
    serial_termios.c_iflag &= ~(INPCK | INLCR | ICRNL | IXON | IXANY | IXOFF);

    /* 8-N-1 */
    serial_termios.c_cflag &= ~(CSIZE | CSTOPB | PARENB);
    serial_termios.c_cflag |= CS8 | CREAD | CLOCAL;

    /* No output processing */
    serial_termios.c_oflag &= ~(OPOST | ONLCR | OCRNL);

    /* Use select() to determine if there is data */
    serial_termios.c_cc[VTIME] = 0;
    serial_termios.c_cc[VMIN] = 0;

    tcflush(g_serial_fd, TCIFLUSH);
    if (tcsetattr(g_serial_fd, TCSANOW, &serial_termios)) {
        syslog(LOG_ERR, "could not set attrs for serial fd: %s\n",
               strerror(errno));
        serial_close();
        return FAILURE;
    }

    return SUCCESS;
}

void serial_close() { close(g_serial_fd); }

int serial_write(const unsigned char *data, int len) {
    return write(g_serial_fd, data, len);
}

int serial_read(unsigned char *buf, int maxlen) {
    return read(g_serial_fd, buf, maxlen);
}

void serial_set_pollin_callback(void (*callback)()) {
    g_serial_pollin_callback = callback;
}

void serial_break_poll_loop() { g_poll_loop_break_flag = 1; }

void serial_start_poll_loop() {
    struct pollfd fds[1];
    fds[0].fd = g_serial_fd;
    fds[0].events = POLLIN;

    while (1) {
        int result = poll(fds, sizeof(fds) / sizeof(struct pollfd),
                          SERIAL_POLL_INTERVAL_MS);
        if (result < 0) {
            syslog(LOG_ERR, "poll() failed: %s", strerror(errno));
            return;
        } else if (result > 0) {
            g_serial_pollin_callback();
        }

        if (g_poll_loop_break_flag) {
            g_poll_loop_break_flag = 0;
            return;
        }
    }
}
