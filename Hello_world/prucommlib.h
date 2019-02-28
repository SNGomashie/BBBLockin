#ifndef _MYLIB_H_
#define _MYLIB_H_

#include <pru_rpmsg.h>
#include <pru_virtqueue.h>
#include <rsc_types.h>

uint8_t rpmsg_init(void);
void send(char *buf, uint16_t leng);
void listen(void);
void echo(void);

#endif
