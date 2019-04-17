/**************************************/
/*           PRU RPMSG LIBRARY        */
/* An attempt at making rpmsg clearer */
/*          By: Sena Gomashie         */
/**************************************/

#ifndef RPMSG_LIB_C
#define RPMSG_LIB_C

#include <pru_rpmsg_lib.h>

// Debugging
// #define GPIO1 0x4804C000
// #define GPIO_CLEARDATAOUT 0x190
// #define GPIO_SETDATAOUT 0x194
// #define USR0 (1<<21)
// #define USR1 (1<<22)
// #define USR2 (1<<23)
// #define USR3 (1<<24)
// unsigned int volatile * const GPIO1_CLEAR = (unsigned int *) (GPIO1 + GPIO_CLEARDATAOUT);
// unsigned int volatile * const GPIO1_SET   = (unsigned int *) (GPIO1 + GPIO_SETDATAOUT);


/* pru_rpmsg_transport is a strcture containing */
/*      vring information for transportation    */
/*    See 'pru_rpmsg.h' & 'pru_virtio_ring.h'   */
struct pru_rpmsg_transport transport;

/* Transportatoin parameters. */
uint16_t src, dst, len;

/* Received message. */
message input;

/* Status of rpmsg. */
uint8_t status = 0;

/* Initialization for rpmsg. */
uint8_t RPMSGinitialize(void){
  /* Status variables. */
  uint8_t init_status, channel_status, status;

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
  while((init_status != PRU_RPMSG_SUCCESS) || (channel_status != PRU_RPMSG_SUCCESS)){
    // Debugging
    // if (init_status != PRU_RPMSG_SUCCESS) {
    //   *GPIO1_SET = USR2;
    // }
    // if (channel_status != PRU_RPMSG_SUCCESS) {
    //   *GPIO1_SET = USR3;
    // }
  }
  status = 1;
}

/* Receive a message from the ARM and return it */
message RPMSGreceive(void){
  /* Status variables */
  uint8_t receive_status;

  /* See if initialization went correct */
  while(status == 1){

    /* Wait until intterupt from ARM */
    while(!(__R31 & HOST_INT));

    /* Receive message if available from ARM */
    receive_status = pru_rpmsg_receive(&transport, &src, &dst, input, &len);

    /* See if receive went corect */
    while(receive_status != PRU_RPMSG_SUCCESS){
      //    Debugging
      // *GPIO1_SET = USR1;
      // *GPIO1_SET = USR2;
    }
    return *input;
  }
}

/* Send mesasge to ARM */
void RPMSGsend(message *output){
  /* Status variable */
  uint8_t send_status;

  /* Send message to ARM using the virtqeues in pru_rpmsg_transport structure */
  send_status = pru_rpmsg_send(&transport, dst, src, output, (sizeof(output) / sizeof(unsigned char)));

  /* See if transmission went correct */
  while(send_status != PRU_RPMSG_SUCCESS){
    //    Debugging
    // *GPIO1_SET = USR1;
    // *GPIO1_SET = USR3;
  }
}

#endif
