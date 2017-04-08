#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "common.h"
#include "checksum.h"
#include "serial_port.h"

#define FRAME_HEADER 0x01
#define FRAME_TRAILER 0x04
#define FRAME_ESCAPE 0x10

int write_bytes(const unsigned char* data, int len) {
    /* Allocate max possible space (escape every byte, with header/trailer) */
    unsigned char* buf = (unsigned char*)malloc(len * 2 + 4);
    if (buf <= 0) {
        syslog(LOG_ERR, "unable to allocate memory for TX buffer: %s", strerror(errno));
        return FAILURE;
    }
    bzero(buf, len * 2 + 4);

    /* Stage 1. Add header */
    int buf_pos = 0;
    buf[buf_pos++] = FRAME_HEADER; /* Header */

    /* Stage 2. Copy data, escaping FRAME_HEADER/TRAILER/ESCAPE */
#define ESCAPE_AND_APPEND_BYTE(byte) \
    do { \
        if ((byte) == FRAME_HEADER || \
            (byte) == FRAME_TRAILER || \
            (byte) == FRAME_ESCAPE) { \
            buf[buf_pos++] = FRAME_ESCAPE; \
        } \
        buf[buf_pos++] = (byte); \
    } while (0);

    for (int data_pos = 0; data_pos < len; data_pos++) {
        ESCAPE_AND_APPEND_BYTE(data[data_pos]);
    }

    /* Stage 3. Checksum with the same escaping procedure */
    unsigned short checksum = crc_xmodem(data, len);
    ESCAPE_AND_APPEND_BYTE((checksum & 0xff00) >> 8);
    ESCAPE_AND_APPEND_BYTE(checksum & 0xff);

    /* Stage 4. Add trailer */
    buf[buf_pos++] = FRAME_TRAILER;

    return serial_write(buf, buf_pos);
}
