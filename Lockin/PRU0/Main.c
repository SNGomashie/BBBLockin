#include <stdint.h>
#include <stdio.h>
#include "pru_peripheral.h"
#include "pru_rpmsg_lib.h"

#define DEBUG_PIN 7

void main(void) {
  /* Clear all outputs */
  __R30 = 0x00000000;
  /*Allow OCP master port access by the PRU so the PRU can read external memories. */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /*************************/
  /* Variable declarations */
  /*************************/
  char* charReceived;  //RPMsg received message

  uint16_t uint16Freq = 0;  // Desired sample frequency
  uint32_t uint32Period = 0;  // sample frequency Hz converted to S

  uint8_t uint8Num_samples = 0;  // Number of samples wanted

  uint16_t uint16ADC = 0;
  /*************************/
  /*************************/



  /***********************/
  /*   Initializations   */
  /***********************/
  RPMSGinitialize();  // Initialize remoteproc messaging framework
  INTCinitialize(7, 1, 1);  // Initialize interrupt controller | sys_evt 7, channel 1, host_int 1
  INTERNCOMinitialize(25);  // Initialize internal communcation | sys_ect 25
  eCAPinitialize();  // Initialize enhanced capture module
  PRCMinitialize();  // Initialize clock for McSPI
  McSPIinitialze(0x5, 0xF, 0x0);  // Initialize McSPI module | 1,5MHz clk, 16bit word, no intterrupts
  LTC1859initialize();  // Initialize LTC1859 adc

  charReceived = RPMSGreceive();  // Receive message from RPMsg
  uint16Freq = atoi(RPMsg_in);  // Convert freq string to freq int
  uint32Period = (1000000000 / uint16Freq) / 5;  // Convert freq int to period int

  IEPinitialize(uint32Period, 1, cmp);  // Initialize IEP timer | received period, increment by 1, compare mode
  /***********************/
  /***********************/



  /***********************/
  /*     Main program    */
  /***********************/
  INTERNCOMpoke();  // Wake up PRU1
  IEPstart();  // Start IEP timer

  /*      Main loop      */
  while(1){
    while(__R31 & (1 << 31)){  // Interrupt polling
      IEPclear();  // Clear IEP cmp register and system event
      uint16ADC = LTC1859readout(0, 1);
      
    }
  }
  __halt();
}
