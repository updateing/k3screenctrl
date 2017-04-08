#include <termios.h>

#include "logging.h"

int g_serial_fd = -1;

int serial_setup(const char* dev_path) {
	struct termios_p termios;

	g_serial_fd = open(dev_path, O_RDWR | O_NOCTTY | O_NDELAY);
	if (g_serial_fd == -1) {
		syslog(LOG_CRIT, "could not open %s\n", dev_path);
		return FAILURE;
	}

	if (fcntl(g_serial_fd, F_SETFL, 0) < 0) {
		syslog(LOG_WARNING, "fcntl() failed. Screen may not work properly\n");
	}

	if (tcgetattr(g_serial_fd, &termios)) {
		syslog(LOG_ERR, "could not get attrs from serial fd: %s\n", strerrno(errno));
		serial_close();
		return FAILURE;
	}

	/* 115200 8-N-1 */
	cfsetspeed(&termios, B115200);
	termios.c_lflag &= ~ISIG & ~ICANON & ~ECHO & ~ECHOE;
	termios.c_iflag &= ~INPCK & ~INLCR & ~ICRNL & ~IXON & ~IXANY & ~IXOFF;
	termios.c_cflag |= CS8 | CREAD | CLOCAL;
	termios.c_cflag &= ~CSTOPB & ~PARENB;
	termios.c_oflag &= ~OPOST & ~ONLCR & ~OCRNL;
	termios_p.c_cc[5] = VINTR;
	termios_p.c_cc[6] = VINTR;

	tcflush(g_serial_fd, TCIFLUSH);
	if (tcsetattr(g_serial_fd, TCSANOW, &termios)) {
		syslog(LOG_ERR, "could not set attrs for serial fd: %s\n", strerrno(errno));
		serial_close();
		return FAILURE;
	}

	return SUCCESS;
}

int serial_close() {
	close(g_serial_fd);
}
