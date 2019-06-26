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
void RPMSGtransmit_16block(int16_t output[248]);
void RPMSGtransmit_16triple(int16_t output[246]);
void RPMSGtransmit_32block(int32_t output[124]);
void RPMSGtransmit_32double(int32_t output[124]);
uint16_t RPMSGcollect16_send(int16_t value);
uint16_t RPMSGcollect32_send(int32_t value);
uint16_t RPMSGtriplecollect16(int16_t adc, int16_t sin, int16_t cos);
uint16_t RPMSGdoublecollect32(int32_t Qa, int32_t In);
#endif /* RPMSG_LIB_H */
