/**************************************/
/*           PRU RPMSG LIBRARY        */
/* An attempt at making rpmsg clearer */
/*          By: Sena Gomashie         */
/**************************************/

#ifndef RPMSG_LIB_C
#define RPMSG_LIB_C

#include "pru_rpmsg_lib.h"
#include "resource_table.h"

// Debugging
#define GPIO1 0x4804C000
#define GPIO_CLEARDATAOUT 0x190
#define GPIO_SETDATAOUT 0x194
#define USR0 (1<<21)
#define USR1 (1<<22)
#define USR2 (1<<23)
#define USR3 (1<<24)
unsigned int volatile * const GPIO1_CLEAR = (unsigned int *) (GPIO1 + GPIO_CLEARDATAOUT);
unsigned int volatile * const GPIO1_SET   = (unsigned int *) (GPIO1 + GPIO_SETDATAOUT);

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

/* Initialization for rpmsg. */
uint8_t RPMSGinitialize(void){
  *GPIO1_CLEAR = USR0;
  *GPIO1_CLEAR = USR1;
  *GPIO1_CLEAR = USR2;
  *GPIO1_CLEAR = USR3;

  /* Status variables. */
  uint8_t init_status, channel_status;

  /* Make sure the Linux drivers are ready for RPMsg communication. */
  status = &resourceTable.rpmsg_vdev.status;
  while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

  /*                    Initialize RPMSG                   */
  /*           'pru_rpmsg.h' for mor information           */
  /* Initialize underlying transport layer data structures */
  init_status = pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0, &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);

  /* Create RPMsg channel */
  channel_status = pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT);

  /* See if error occured */
  while((init_status != PRU_RPMSG_SUCCESS) && (channel_status != PRU_RPMSG_SUCCESS)){
    // Debugging
    // if (init_status != PRU_RPMSG_SUCCESS) {
    //   *GPIO1_SET = USR2;
    // }
    // if (channel_status != PRU_RPMSG_SUCCESS) {
    //   *GPIO1_SET = USR3;
    // }
  }
  return state = 1;
}

/* Receive a message from the ARM and return it */
char* RPMSGreceive(void){
  /* See if initialization went correct */
  if(state == 1){

    /* Wait until intterupt from ARM */
    while(!(__R31 & HOST_INT));

    /* See if receive went corect */
    while(pru_rpmsg_receive(&transport, &src, &dst, input, &len) != PRU_RPMSG_SUCCESS){
         // Debugging
      // *GPIO1_SET = USR1;
      // *GPIO1_SET = USR2;
    }
    INTCclear(17);
  }
  return input;
}

/* Send mesasge to ARM */
void RPMSGtransmit(char* output){
  uint8_t transmit_status;

  transmit_status = pru_rpmsg_send(&transport, dst, src, output, (strlen(output)));

  /* See if transmission went correct */
  while(transmit_status != PRU_RPMSG_SUCCESS){
    //    Debugging
    // *GPIO1_SET = USR1;
    // *GPIO1_SET = USR3;
  }
}

void RPMSGtransmit_16block(uint16_t output[248]){
  uint8_t blk_transmit_status;
  uint16_t *ptrBLK = output;

  blk_transmit_status = pru_rpmsg_send(&transport, dst, src, ptrBLK, 496);

  /* See if transmission went correct */
  while(blk_transmit_status != PRU_RPMSG_SUCCESS){
    //    Debugging
    // *GPIO1_SET = USR1;
    // *GPIO1_SET = USR3;
  }

}

void RPMSGtransmit_32block(uint32_t output[124]){
  uint8_t blk_transmit_status;
  uint32_t *ptrBLK = output;

  blk_transmit_status = pru_rpmsg_send(&transport, dst, src, ptrBLK, 496);

  /* See if transmission went correct */
  while(blk_transmit_status != PRU_RPMSG_SUCCESS){
    //    Debugging
    // *GPIO1_SET = USR1;
    // *GPIO1_SET = USR3;
  }

}

uint16_t blk16data[248];
uint8_t var16 = 0;
uint8_t pack16 = 0;

/* Will send a block of 248 16-bit numbers over RPMsg */
uint8_t RPMSGcollect16_send(uint16_t value){
  blk16data[var16] = value;
  var16++;
  if(!(var16 < 248)){
    RPMSGtransmit_16block(blk16data);
    var16 = 0;
    pack16++;
  }
  return pack16;
}

uint32_t blk32data[124];
uint8_t var32 = 0;
uint8_t pack32 = 0;

/* Will send a block of 124 32-bit numbers over RPMsg */
uint8_t RPMSGcollect32_send(uint16_t value){
  blk32data[var32] = value;
  var32++;
  if(!(var32 < 124)){
    RPMSGtransmit_32block(blk32data);
    var32 = 0;
    pack32++;
  }
  return pack32;
}
#endif
