/*       Made by Sena Gomashie 20-05-2019       */
/*              For SRON Groningen              */
/*      numerically Controlled Oscillator       */
/*                 Example:                     */
/*      Generate a 10Hz wave using a NCO        */
/*         Print the wave via UART              */

#include "stdint.h"
#include "stdio.h"
#include "RPMsg.h"
#include "GPIO.h"
#include "NCO.h"
#include "INTC.h"
#include "IEP.h"


void main(void){
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;  // Allow OCP master port access by the PRU so the PRU can read external memories.
  char* charReceived;  //RPMsg received message
  uint32_t uint32command;  // ARM command
  uint16_t uint16Freq = 0;  // Desired sample frequency
  uint16_t uint16packets = 0;  // Number of samples wanted
  uint32_t uint32Period = 0;  // sample frequency Hz converted to S
  struct NCO oscReference;
  char data[] = "";

  RPMSGinitialize();

  charReceived = RPMSGreceive();
  uint32command = atoi(charReceived);  // Convert command string to int
  uint16packets = (uint32command >> 16);  // Extract encoded number of packets
  uint16Freq = (uint32command & 0xFFFF);  // Extract encoded frequency

  NCOinitialize(&oscReference, (100000000 / 100));


  INTC_initialize(7, 1, 1);  // Initialize IEP system event
  IEP_reset();  // Reset IEP
  IEP_mode(0);  // Go into compare mode
  IEP_frequency(uint16Freq);  // Set sample frequency
  IEP_increment(1);  // Set increment to 1
  IEP_start();  // Start IEP timer

  while(1){
    while (__R31 & (1 << 31)){
      IEP_clear();  // Clear IEP compare event
      INTC_clear(7);  // Clear IEP system event

      NCOsetfreq(&oscReference);  // Change the tuning word to stay in-phase
      NCOstep(&oscReference);  // Go to the next value of the sin wave

      if(RPMSGcollect16_send(oscReference.sin_output) == uint16packets){
        IEP_stop();
      }
    }
  }

}
