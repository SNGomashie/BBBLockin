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

/* Initialization for rpmsg. */
uint8_t RPMSGinitialize(void){
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
  while((init_status != PRU_RPMSG_SUCCESS) || (channel_status != PRU_RPMSG_SUCCESS)){
    // Debugging
    if (init_status != PRU_RPMSG_SUCCESS) {
      *GPIO1_SET = USR2;
    }
    if (channel_status != PRU_RPMSG_SUCCESS) {
      *GPIO1_SET = USR3;
    }
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
      //    Debugging
      // *GPIO1_SET = USR1;
      // *GPIO1_SET = USR2;
    }
  }
  return input;
}

/* Send mesasge to ARM */
void RPMSGsend(char* output){
  /* See if transmission went correct */
  while(pru_rpmsg_send(&transport, dst, src, output, (strlen(output))) != PRU_RPMSG_SUCCESS){
    //    Debugging
    // *GPIO1_SET = USR1;
    // *GPIO1_SET = USR3;
  }
}

#endif