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
uint16_t sqrt(uint32_t x);
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
  char* charReceived;  //RPMsg received message
  uint32_t uint32command;  // ARM command

  uint16_t uint16Freq = 0;  // Desired sample frequency
  uint32_t uint32Period = 0;  // sample frequency Hz converted to S

  uint16_t uint16packets = 0;  // Number of samples wanted

  int16_t int16ADC = 0;  // ADC output
  uint16_t uint16Cos = 0;  // Cos output DDS
  uint16_t uint16Sin = 0;  // Sin output DDS

  int32_t int32Q, int32I, int32R = 0;  // Quadrature, In-phase and Magnitude
  uint64_t int64Qpow, int64Ipow = 0;
  uint16_t uint16Navr = 1000;  // Integration time
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
  LTC1859initialize();  // Initialize LTC1859 adc

  charReceived = RPMSGreceive();  // Receive message from RPMsg
  uint32command = atoi(charReceived);  // Convert command string to int
  uint16packets = (uint32command >> 16);  // Extract encoded number of packets
  uint16Freq = (uint32command & 0xFFFF);  // Extract encoded frequency
  uint32Period = (1000000000 / uint16Freq) / 5;  // Convert freq int to period int

  IEPinitialize(uint32Period, 1, cmp);  // Initialize IEP timer | received period, increment by 1, compare mode
  /*************************/
  /*************************/



  /*************************/
  /*      Main program     */
  /*************************/
  sMEM[2] = uint32Period / 100;
  INTERNCOMpoke(PRU0_PRU1_START_INT);  // Wake up PRU1
  IEPstart();  // Start IEP timer


  /*       Main loop       */
  while(1){
    while(__R31 & (1 << 31)){  // IEP interrupt polling
      IEPclear();  // Clear IEP cmp register and system event

      int16ADC = LTC1859readout(0, 0);  // Read a sample form the LTC1859

      INTERNCOMlisten(1, PRU1_PRU0_SEND_INT);  // Wait for NCO to be done on PRU1

      uint16Sin = sMEM[0];  // SIN is located in reg 0 of the shared memory
      uint16Cos = sMEM[1];  // COS is located in reg 1 of the shared memory

      // int32Q = (uint32_t)(uint16_t)uint16Sin * (uint32_t)(int16_t)int16ADC;
      // int32I = (uint32_t)(uint16_t)uint16Cos * (uint32_t)(int16_t)int16ADC;

      /* Quadrature calculation and moving average filtering */
      int32Q -= int32Q / uint16Navr;
      int32Q += ((uint32_t)(uint16_t)uint16Sin * (int32_t)(int16_t)int16ADC) / uint16Navr;
      //
      // /* In-phase calculation and moving average filtering */
      int32I -= int32I / uint16Navr;
      int32I += ((uint32_t)(uint16_t)uint16Cos * (int32_t)(int16_t)int16ADC) / uint16Navr;
      //
      // /* Magnitude calculation and moving avergae filtering */
      int32R -= int32R / uint16Navr;
      //
      int64Qpow = (uint64_t)abs(int32Q) * (uint64_t)abs(int32Q);  // Calculate Q sqaured using MAC
      int64Ipow = (uint64_t)abs(int32I) * (uint64_t)abs(int32I);  // Calculate I squared using MAC
      //
      int32R += sqrt(int64Qpow + int64Ipow) / uint16Navr;  // Magnitude calculation

      if(RPMSGcollect32_send(int32R) == uint16packets){
        IEPstop();
      }
    }
  }
 /************************/


  __halt();
}
 /************************/
 /************************/



 uint16_t sqrt(uint32_t x){
 	uint32_t fa,sa,ta;
 	uint32_t error,error_last;

   // Since the squareroot of X will always be smaller than X / 2
 	// We can choose an (arbitrary) first approach as X / 2
 	// The closer this number is to the final answer the faster this routines completes.

 	fa = x>>1;		// Divide number by 2, bitshifting performs the same operation

 	// Divide the argument number by the first approach
 	sa = x/fa;

 	// Get the mean between the two previous numbers (add them and divide by 2).
 	ta = (fa+sa)>>1;

 	error_last=-1;
 	error=0;

 	/*
 	 * Repeat this routine until the integer output value is no longer changing.
 	 */
 	do {
 		error_last=error;
 		fa = ta;
 		sa = x/fa;
 		ta = (fa+sa)>>1;
 		error=abs(x - ta*ta);
 	} while (error_last!=error);

 	// Return the integer result: square root of the input argument x.
 	return ta;
 }
