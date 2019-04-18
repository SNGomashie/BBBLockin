/**************************************/
/*           PRU RPMSG LIBRARY        */
/* An attempt at making rpmsg clearer */
/*          By: Sena Gomashie         */
/**************************************/

#ifndef __RPMSG_LIB_H
#define __RPMSG_LIB_H

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

/* Prototype functions */
uint8_t RPMSGinitialize(void);
char* RPMSGreceive(void);
void RPMSGtransmit(char* output);
void RPMSGclear_int(void);

#endif /* RPMSG_LIB_H */
