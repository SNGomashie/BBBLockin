#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "pru_peripheral.h"
#include "LTC1859.h"
#include "pru_rpmsg_lib.h"



/**************************/
/*       Definitions      */
/**************************/
#define DEBUG_PIN 3  // Pin used for debugging | P9_28
#define PRU0_PRU1_START_INT (13+16)  // Interrupt for starting PRU1 | sys_evt 29, channel 0, host_int 0
#define PRU1_PRU0_SEND_INT (14+16)  // Interrupt for transmitting to PRU0 | sys_evt 30, channel 1, host_int 1
#define PRU0_PRU1_DONE_INT (15+16)  // Interrupt for stopping PRU1 | sys_evt 31, channel 0, host_int 0
#define PRU_ICSS_IEP_INT (7)  // Interrupt from IEP timer | sys_evt 7, channel 1, host_int 1
/**************************/
/**************************/



/*****************************/
/*    Function Prototypes    */
/*****************************/

/*****************************/
/*****************************/



/*****************************/
/* PRU-ICSS Memory locations */
/*****************************/
#define SHARE_MEM  0x00010000  // 12kB of shared memory
volatile uint32_t *sMEM =  (unsigned int *) SHARE_MEM;

#define PRU0_MEM  0x00000000  // 8kB of Data RAM
volatile uint32_t *dMEM0 =  (unsigned int *) PRU0_MEM;

#define PRU1_MEM  0x00002000  // 8kB of Data RAM of the secondary PRU
volatile uint32_t *dMEM1 =  (unsigned int *) PRU1_MEM;
/*****************************/
/*****************************/



void main(void) {
    __R30 = 0x00000000;  // Clear all the output pins

  /*************************/
  /* Variable declarations */
  /*************************/
  char* Received;  //RPMsg received message
  uint32_t command;  // ARM command

  uint16_t Freq = 0;  // Desired sample frequency
  uint32_t Period = 0;  // sample frequency Hz converted to S

  int16_t packets = 0;  // Number of samples wanted

  int16_t adc = 0;  // ADC output
  int16_t Cos = 0;  // Cos output DDS
  int16_t Sin = 0;  // Cos output DDS

  uint8_t Nch = 0;
  uint32_t ch = 0;

  int32_t Qa, In = 0;  // Quadrature, In-phase and Magnitude

  /*************************/
  /*************************/



  /*************************/
  /*    Initializations    */
  /*************************/
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;  // Allow OCP master port access by the PRU so the PRU can read external memories.

  RPMSGinitialize();  // Initialize remoteproc messaging framework
  INTCinitialize(PRU_ICSS_IEP_INT, 1, 1);  // Initialize interrupt controller | sys_evt 7, channel 1, host_int 1
  INTCinitialize(PRU0_PRU1_START_INT, 0, 0);  // Initialize interrupt controller | sys_evt 29, channel 0, host_int 0
  INTCinitialize(PRU0_PRU1_DONE_INT, 0, 0);  // Initialize interrupt controller | sys_evt 31, channel 0, host_int 0
  PRCMinitialize();  // Initialize clock for McSPI
  McSPIinitialze(0x5, 0xF, 0x0);  // Initialize McSPI module | 1,5MHz clk, 16bit word, no intterrupts
  LTC1859initialize();  // Initialize LTC1859 ADC

  Received = RPMSGreceive();  // Receive message from RPMsg
  command = atoi(Received);  // Convert command string to int
  Nch = (command >> 16);  // Extract encoded number of packets
  Freq = (command & 0xFFFF);  // Extract encoded frequency
  Period = (1000000000 / Freq) / 5;  // Convert freq int to period int

  IEPinitialize(Period, 1, cmp);  // Initialize IEP timer | received period, increment by 1, compare mode
  /*************************/
  /*************************/



  /*************************/
  /*      Main program     */
  /*************************/
  if (packets == 0){
    packets = -1;
  }
  sMEM[2] = Period / 100;
  INTERNCOMpoke(PRU0_PRU1_START_INT);  // Wake up PRU1
  IEPstart();  // Start IEP timer

  /*       Main loop       */
  while(1){
      while(__R31 & (1 << 31)){  // IEP interrupt polling
        IEPclear();  // Clear IEP cmp register and system event

        ch++;  // Cycle through channels
        adc = LTC1859readout((ch % Nch), 0);  // Read a sample form the LTC1859

        INTERNCOMlisten(1, PRU1_PRU0_SEND_INT);  // Wait for NCO to be done on PRU1

        Sin = sMEM[0];  // SIN is located in reg 0 of the shared memory
        Cos = sMEM[1];  // COS is located in reg 1 of the shared memory

        Qa = (Sin * adc) >> 14;
        In = (Cos * adc) >> 14;

        if(RPMSGdoublecollect32(Qa, In) == packets){
          IEPstop();
        }
      }
  }

 /************************/


  __halt();
}
 /************************/
 /************************/
