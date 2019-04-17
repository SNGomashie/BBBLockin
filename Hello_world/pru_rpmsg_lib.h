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
#include "resource_table.h"


/* Used to make sure the Linux drivers are ready for RPMsg communication */
#define VIRTIO_CONFIG_S_DRIVER_OK	4

#define RPMSG_HEADER_SIZE			16

/* Create array type for messaging */
typedef struct{
  unsigned char x[RPMSG_BUF_SIZE - RPMSG_HEADER_SIZE];
} message;

/* Prototype functions */
void RPMSGinitialize(void);
message RPMSGreceive(void);
void RPMSGsend(char *output);
void RPMSGclear_int(void);

#endif /* RPMSG_LIB_H */
