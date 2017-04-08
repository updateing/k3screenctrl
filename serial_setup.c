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

	cfsetispeed(&termios, B115200); // Wow magic
	cfsetospeed(&termios, B115200);
	termios.c_lflag &= ~ECHOKE & ~ECHOE & ~ECHO & ~ECHONL;
	termios.c_iflag &= ~PARMRK & ~ISTRIP & ~INLCR & ~ICRNL & ~IXON; // 0x800?
	/*
	    cfsetispeed(&termios_p, 0x1002u);           // WTF? StackOverflow says it's B115200 but can't find out why
    cfsetospeed(&termios_p, 0x1002u);
    termios_p.c_lflag &= 0xFFFFFFE4;            // No ECHOKE/ECHOE/ECHO/ECHONL !!! Different among C libs !!!
    termios_p.c_iflag &= 0xFFFFE2AF;            // No INPCK/INLCR/ICRNL/IXOFF/IXANY/1<<11
    termios_p.c_cflag = (termios_p.c_cflag | 0x8B0) & 0x7FFFFEBF;// Add CREAD/1<<4,5,7, No CS6/1<<6/0x80000000 7-N-1
    termios_p.c_oflag &= 0xFFFFFFF2;            // No OPOST/OXTABS/ONOEOT
    termios_p.c_cc[5] = 0;                      // VINTR
    termios_p.c_cc[6] = 0;
    */
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