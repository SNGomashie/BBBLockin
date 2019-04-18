/**************************************/
/*           PRU RPMSG LIBRARY        */
/* An attempt at making rpmsg clearer */
/*          By: Sena Gomashie         */
/**************************************/

#ifndef RPMSG_LIB_H
#define RPMSG_LIB_H

#include <pru_rpmsg.h>
#include <pru_virtqueue.h>
#include <rsc_types.h>
#include "pru_virtio_ids.h"


#ifndef __R31
  volatile register uint32_t __R31;
#endif

/* Used to make sure the Linux drivers are ready for RPMsg communication */
#define VIRTIO_CONFIG_S_DRIVER_OK	4

#define RPMSG_HEADER_SIZE			16

/* Create array type for messaging */
typedef struct{
  unsigned char x[RPMSG_BUF_SIZE - RPMSG_HEADER_SIZE];
} message;

/* pru_rpmsg_transport is a strcture containing */
/*      vring information for transportation    */
/*    See 'pru_rpmsg.h' & 'pru_virtio_ring.h'   */
struct pru_rpmsg_transport transport;

/* Transportation parameters. */
uint16_t src, dst, len;

/* Received message. */
char* input;

/* Status of rpmsg. */
volatile uint8_t *status;
uint8_t state;

/* Prototype functions */
uint8_t RPMSGinitialize(void);
char* RPMSGreceive(void);
void RPMSGsend(char* output);
void RPMSGclear_int(void);

#endif /* RPMSG_LIB_H */
