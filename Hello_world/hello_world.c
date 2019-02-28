#include <stdint.h>
#include <pru_cfg.h>
#include <pru_rpmsg.h>
#include <pru_virtqueue.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include "resource_table.h"

/* Used to make sure the Linux drivers are ready for RPMsg communication */
#define VIRTIO_CONFIG_S_DRIVER_OK	4

/* Define the size of message to be recieved. */
#define RPMSG_BUF_HEADER_SIZE           16
uint8_t rec_payload[RPMSG_BUF_SIZE - RPMSG_BUF_HEADER_SIZE];

/* Global variable definitions */
volatile register uint32_t __R31;
struct pru_rpmsg_transport transport;
uint16_t src, dst, len;

void main (void) {
  volatile uint8_t *status;
  /*Allow OCP master port access by the PRU so the PRU can read external memories. */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* Make sure the Linux drivers are ready for RPMsg communication. */
  status = &resourceTable.rpmsg_vdev.status;
  while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

  /* Initialize the RPMsg transport structure */
  while (pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0, &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST) != PRU_RPMSG_SUCCESS);

  /* Create the RPMsg channel between the PRU and ARM user space using the transport structure. */
  while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);

  /* Receive all available messages, multiple messages can be sent per kick. A message has to be received to set the destination adress before you send. */
  while (pru_rpmsg_receive(&transport, &src, &dst, rec_payload, &len) != PRU_RPMSG_SUCCESS);  //Initialize the RPMsg framework

  while(1) {
  /* Send chars to the ARM, buf = payload, 11 is length of payload. */
    char *buf = "hallo world";
    pru_rpmsg_send(&transport, dst, src, buf, 11);
    __delay_cycles(200000000);    // Wait 1/2 second
  }
}
