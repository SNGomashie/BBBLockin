#include <stdint.h>
#include <pru_cfg.h>
#include <pru_rpmsg.h>
#include <pru_virtqueue.h>
#include <pru_intc.h>
#include <sys_tscAdcSs.h>
#include <rsc_types.h>
#include "eprintf.h"
#include "resource_table_0.h"

/* Used to make sure the Linux drivers are ready for RPMsg communication */
#define VIRTIO_CONFIG_S_DRIVER_OK	4

/* Define the size of message to be recieved. */
#define RPMSG_BUF_HEADER_SIZE           16
uint8_t rec_payload[RPMSG_BUF_SIZE - RPMSG_BUF_HEADER_SIZE];

/* Control Module registers to enable the ADC peripheral */

#define CM_WKUP_CLKSTCTRL  (*((volatile unsigned int *)0x44E00400))
#define CM_WKUP_ADC_TSC_CLKCTRL  (*((volatile unsigned int *)0x44E004BC))

void init_adc();
uint16_t read_adc(void);

volatile register uint32_t __R31;
struct pru_rpmsg_transport transport;
uint16_t src, dst, len, voltage;
char buffer[3];

void main(void) {
  volatile uint8_t *status;

  /*Allow OCP master port access by the PRU so the PRU can read external memories. */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  init_adc();

  /*
	 * Clear the status of the PRU-ICSS system event that the ARM will
	 * use to 'kick' us
	 */
	CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;

  /* Make sure the Linux drivers are ready for RPMsg communication. */
  status = &resourceTable.rpmsg_vdev.status;
  while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

  /* Initialize the RPMsg transport structure */
  while (pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0, &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST) != PRU_RPMSG_SUCCESS);

  /* Create the RPMsg channel between the PRU and ARM user space using the transport structure. */
  while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);

  while(1){
    /* Check register R31 bit 30 to see if the ARM has kicked us */
    if (!(__R31 & HOST_INT))
      continue;

    /* Clear the event status */
    CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;

    /*
		 * Receive available messages.
		 * Multiple messages can be sent per kick.
		 */
		while (pru_rpmsg_receive(&transport, &src, &dst,
				rec_payload, &len) == PRU_RPMSG_SUCCESS) {

			/* ARM sends a message using shared_struct */

			voltage = read_adc();
      esprintf(buffer,"%03X", voltage);
			/*
			 * Send reply message to the address that sent
			 * the initial message
			 */
			pru_rpmsg_send(&transport, dst, src, buffer,
				sizeof(buffer));
		}
  }
}

void init_adc(){

	/* set the always on clock domain to NO_SLEEP. Enable ADC_TSC clock */
	while (!(CM_WKUP_ADC_TSC_CLKCTRL == 0x02)) {
		CM_WKUP_CLKSTCTRL = 0;
		CM_WKUP_ADC_TSC_CLKCTRL = 0x02;
		/* Optional: implement timeout logic. */
	}

	/*
	 * Set the ADC_TSC CTRL register.
	 * Disable TSC_ADC_SS module so we can program it.
	 * Set step configuration registers to writable.
	 */
	ADC_TSC.CTRL_bit.ENABLE = 0;
	ADC_TSC.CTRL_bit.STEPCONFIG_WRITEPROTECT_N_ACTIVE_LOW = 1;

	/*
	 * set the ADC_TSC STEPCONFIG1 register for channel 5
	 * Mode = 0; SW enabled, one-shot
	 * Averaging = 0x3; 8 sample average
	 * SEL_INP_SWC_3_0 = 0x4 = Channel 5
         * SEL_INM_SWC_3_0 = 1xxx = VREFN (reduces noise in single ended mode)
	 * use FIFO0
	 */
	ADC_TSC.STEPCONFIG1_bit.MODE = 0;
	ADC_TSC.STEPCONFIG1_bit.AVERAGING = 3;
	ADC_TSC.STEPCONFIG1_bit.SEL_INP_SWC_3_0 = 4;
	ADC_TSC.STEPCONFIG1_bit.SEL_INM_SWC_3_0 = 8;
	ADC_TSC.STEPCONFIG1_bit.FIFO_SELECT = 0;

	/*
	 * set the ADC_TSC CTRL register
	 * set step configuration registers to protected
	 * store channel ID tag if needed for debug
	 * Enable TSC_ADC_SS module
	 */
	ADC_TSC.CTRL_bit.STEPCONFIG_WRITEPROTECT_N_ACTIVE_LOW = 0;
	ADC_TSC.CTRL_bit.STEP_ID_TAG = 1;
	ADC_TSC.CTRL_bit.ENABLE = 1;
}

uint16_t read_adc(void) {
	/*
	 * Clear FIFO0 by reading from it
	 * We are using single-shot mode.
	 * It should not usually enter the for loop
	 */
	uint32_t count = ADC_TSC.FIFO0COUNT;
	uint32_t data;
	uint32_t i;
	for (i = 0; i < count; i++) {
		data = ADC_TSC.FIFO0DATA;
	}

	/* read from the ADC channel 5*/
			ADC_TSC.STEPENABLE_bit.STEP1 = 1;


	while (ADC_TSC.FIFO0COUNT == 0) {
		/*
		 * loop until value placed in fifo.
		 * Optional: implement timeout logic.
		 *
		 * Other potential options include:
		 *   - configure PRU to receive an ADC interrupt. Note that
		 *     END_OF_SEQUENCE does not mean that the value has been
		 *     loaded into the FIFO yet
		 *   - perform other actions, and periodically poll for
		 *     FIFO0COUNT > 0
		 */
	}

	/* read the voltage */
	uint16_t voltage = ADC_TSC.FIFO0DATA_bit.ADCDATA;

	return voltage;
}
