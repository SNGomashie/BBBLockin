#include <stdint.h>
#include <pru_cfg.h>
#include <pru_rpmsg.h>
#include <pru_virtqueue.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <math.h>
#include "eprintf.h"
#include "resource_table.h"


/* Used to make sure the Linux drivers are ready for RPMsg communication */
#define VIRTIO_CONFIG_S_DRIVER_OK	4

/* Data object to be send through the scratchpad */
typedef struct buffer{
  uint16_t reg0;
  uint16_t reg1;
} bufferData;

bufferData dmemBuf;

/* Define the size of message to be recieved. */
#define RPMSG_BUF_HEADER_SIZE           16
uint8_t rec_payload[RPMSG_BUF_SIZE - RPMSG_BUF_HEADER_SIZE];

/* Interrupt definitions */
#define INT_OFF 0x00000000
#define INT_ON 0xFFFFFFFF

/* Shared memory location & defintions */
#define SHARE_MEM  0x00010000
volatile uint32_t *shared =  (unsigned int *) SHARE_MEM;

/* Global variable definitions */
volatile register uint32_t __R30;
volatile register uint32_t __R31;
struct pru_rpmsg_transport transport;
uint16_t src, dst, len;
char buffer[20];
const float SCALE = (10 / pow(2, 16));

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

  /* Inifinite loop */
  while(1){
    while(shared[0] == INT_ON){
      /* Read scratchpad */
      __xin(10, 0, 0, dmemBuf);

      uint16_t v1 = SCALE * dmemBuf.reg0);
      uint16_t v2 = SCALE * dmemBuf.reg1);

      /* Compose the string to be send */
      esprintf(buffer,"%04X,%04X\n", v1, v2);

      /* Send message to ARM using RPMSG, buffer is the payload, 20 is the length of the payload */
      pru_rpmsg_send(&transport, dst, src, buffer, 10);

      /* reset shared memory interrupt*/
      shared[0] = INT_OFF;

      /* Delay half a second */
      __delay_cycles(200000000);
    }
  }

}
