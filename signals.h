#ifndef _SIGNALS_H
#define _SIGNALS_H

/**
 * Set up signalfd containing SIGTERM/USR1/USR2.
 *
 * Return the signalfd, or -1 for failure.
 */
int signal_setup();

/**
 * Notify there is a signal.
 */
void signal_notify();

#endif