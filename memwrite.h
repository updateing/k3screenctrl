#ifndef _MEMWRITE_H
#define _MEMWRITE_H

/*
 * Write one byte to specific physical address.
 * Requires /dev/mem support.
 */
int write_memory_byte(void* addr, unsigned char data);

#endif