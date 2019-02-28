#include <stdint.h>
#include <string.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include "resource_table.h"
#include "prucommlib.h"

//defines
#define VIRTIO_CONFIG_S_DRIVER_OK	4

#define RPMSG_BUF_HEADER_SIZE           16
uint8_t rec_payload[RPMSG_BUF_SIZE - RPMSG_BUF_HEADER_SIZE];

//global variable definitions
volatile register uint32_t __R31;
struct pru_rpmsg_transport transport;
uint16_t src, dst, len;
unsigned char rpmsg_errno;

uint8_t rpmsg_init(void){
  volatile uint8_t *status;
  static unsigned short rpmsg_state=0;
  CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;
  status = &resourceTable.rpmsg_vdev.status;
  while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));
  pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0, &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);

  while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);
  while (pru_rpmsg_receive(&transport, &src, &dst, rec_payload, &len) != PRU_RPMSG_SUCCESS);
  rpmsg_state = 1;
  return(rpmsg_state);
}

void send(char *buf, uint16_t leng){
    pru_rpmsg_send(&transport, dst, src, buf, leng);
}

void listen(void) {
  pru_rpmsg_receive(&transport, &src, &dst, rec_payload, &len);
}

void echo(void){

    if (__R31 & HOST_INT) {
      while ((pru_rpmsg_receive(&transport, &src, &dst, rec_payload, &len)) == PRU_RPMSG_SUCCESS){
        pru_rpmsg_send(&transport, dst, src, rec_payload, len);
      }
    }
  }
