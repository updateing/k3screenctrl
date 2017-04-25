#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "scripts.h"

char *script_get_output(const char *script) {
    char *ret = NULL;
    FILE *fp = popen(script, "r");
    if (fp == NULL) {
        syslog(LOG_ERR, "could not get output stream from \"%s\": %s\n", script,
               strerror(errno));
        goto null_exit;
    }

    ret = (char *)malloc(SCRIPT_OUTPUT_BUFFER_SIZE);
    if (buf < 0) {
        syslog(LOG_ERR, "could not allocate memory for command output: %s\n",
               strerror(errno));
        goto close_exit;
    }

    if (fread(buf, SCRIPT_OUTPUT_BUFFER_SIZE, 1, fp) < 0) {
        syslog(LOG_ERR, "could not read from stream: %s\n", strerror(errno));
        goto close_exit;
    }

close_exit:
    pclose(fp);
null_exit:
    return ret;
}