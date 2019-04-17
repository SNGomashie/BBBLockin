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

/* Create array type for messaging */
typedef struct message{
  unsigned char x[RPMSG_BUF_SIZE - RPMSG_HEADER_SIZE];
};

/* Prototype functions */
void RPMSGinitialize(void);
message RPMSGreceive(void);
void RPMSGsend(message *output);
void RPMSGclear_int(void);

#endif /* RPMSG_LIB_H */