#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "pru_peripheral.h"
#include "pru_rpmsg_lib.h"

/* PIN number used for debugging */
#define DEBUG_PIN 3

void main(void) {
  /************************/
  /*        SETUP         */
  /************************/
  __R30 = 0x00000000;  // Clear al output pins
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;  // Allow OCP master port access by the PRU so the PRU can read external memories.
  /************************/
  /************************/



  /*************************/
  /* Variable declarations */
  /*************************/
  char* charReceived;  //RPMsg received message

  uint16_t uint16Freq = 0;  // Desired sample frequency
  uint32_t uint32Period = 0;  // sample frequency Hz converted to S

  uint8_t uint8packets = 0;  // Number of samples wanted

  uint16_t uint16ADC = 0;  // ADC output
  uint16_t uint16Cos = 0;  // Cos output DDS
  uint16_t uint16Sin = 0;  // Sin output DDS

  uint32_t uint32Q, uint32I, uint32R = 0;  // Quadrature, In-phase and Magnitude
  uint64_t uint64Qpow, uint64Ipow = 0;
  uint16_t uint16Navr = 1000;  // Integration time
  /*************************/
  /*************************/



  /*************************/
  /*    Initializations    */
  /*************************/
  RPMSGinitialize();  // Initialize remoteproc messaging framework
  INTCinitialize(7, 1, 1);  // Initialize interrupt controller | sys_evt 7, channel 1, host_int 1
  INTERNCOMinitialize(25);  // Initialize internal communcation | sys_evt 25
  PRCMinitialize();  // Initialize clock for McSPI
  McSPIinitialze(0x5, 0xF, 0x0);  // Initialize McSPI module | 1,5MHz clk, 16bit word, no intterrupts
  LTC1859initialize();  // Initialize LTC1859 adc

  charReceived = RPMSGreceive();  // Receive message from RPMsg
  uint16Freq = atoi(RPMsg_in);  // Convert freq string to freq int
  uint32Period = (1000000000 / uint16Freq) / 5;  // Convert freq int to period int

  IEPinitialize(uint32Period, 1, cmp);  // Initialize IEP timer | received period, increment by 1, compare mode
  /*************************/
  /*************************/



  /*************************/
  /*      Main program     */
  /*************************/
  INTERNCOMpoke();  // Wake up PRU1
  sRAM[3] = uint32Period / 100;
  IEPstart();  // Start IEP timer


  /*       Main loop       */
  while(1){
    while(__R31 & (1 << 31)){  // IEP interrupt polling
      IEPclear();  // Clear IEP cmp register and system event

      uint16ADC = LTC1859readout(0, 1);  // Read a sample form the LTC1859

      INTERNCOMlisten();  // Wait for DDS to be done on PRU1
      INTERCOMpoke();  // Respond to PRU1


      /* TODO: is this gonna be shared memory or scratchpad? */
      uint16Sin = sRAM[0] & 0xFFFF0000;  // SIN is 16 MSB of sRAM reg 0
      uint16Cos = sRAM[0] & 0x0000FFFF;  // COS is 16 LSB of sRAM reg 0

      /* Quadrature calculation and moving average filtering */
      uint32Q -= uint32Q / uint16Navr;
      uint32Q += (uint16Sin * uint16ADC) / uint16Navr;

      /* In-phase calculation and moving average filtering */
      uint32I -= uint32I / uint16Navr;
      uint32I += (uint16Cos * uint16ADC) / uint16Navr;

      /* Magnitude calculation and moving avergae filtering */
      uint32R -= uint32R / uint16Navr;

      uint64Qpow = (uint64)uint32Q * (uint64)uint32Q;  // Calculate Q sqaured using MAC
      uint64Ipow = (uint64)uint32I * (uint64)uint32I;  // Calculate I squared using MAC

      uint32R += sqrt(uint64Qpow + uint64Ipow) / uint16Navr;  // Magnitude calculation

      if(RPMSGcollect_send(uint32R) == uint8packets){
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
 		error=iabs(x - ta*ta);
 	} while (error_last!=error);

 	// Return the integer result: square root of the input argument x.
 	return ta;
 }
