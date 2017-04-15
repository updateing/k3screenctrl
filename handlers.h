#ifndef _HANDLERS_H
#define _HANDLERS_H

typedef struct _response_handler {
    RESPONSE_TYPE type;
    void (*handler)(unsigned char* payload, int len);
} RESPONSE_HANDLER;

#endif
