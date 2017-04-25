#ifndef _SERIAL_PORT_H
#define _SERIAL_PORT_H

#define SERIAL_POLL_INTERVAL_MS 500

/*
 * Setup serial port at given path
 *
 * Returns: SUCCESS, FAILURE
 */
int serial_setup(const char *dev_path);

/*
 * Close the port
 */
void serial_close();

/*
 * Self-explanatory
 *
 * Returns: how many bytes are actually written / read
 */
int serial_write(const unsigned char *data, int len);
int serial_read(unsigned char *buf, int maxlen);

/*
 * Callback when there is new data available to read.
 * Use serial_read() to get the data.
 *
 * Note: does not guarantee there is a complete frame.
 * It may take several callbacks to receive a complete
 * one.
 */
void serial_set_pollin_callback(void (*callback)());

/*
 * Self-explanatory.
 *
 * Note: serial_start_poll_loop() will start the loop
 * in current thread!
 */
void serial_break_poll_loop();
void serial_start_poll_loop();

#endif
