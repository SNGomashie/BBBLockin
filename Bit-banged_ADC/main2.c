//  Attempt to read the
//  SPI ADC read program in C.
//
//	Pin configuration:
//  nRD (  Read Input ) :						P9.25 pr1_pru0_pru_r30_7
//  CS	( chip select ):  					P9.27 pr1_pru0_pru_r30_5
//  SDO ( Serial Data Out (MISO)):  P9.28 pr1_pru0_pru_r31_3
//  SDI ( Serial Data In (MOSI)):   P9.29 pr1_pru0_pru_r30_1
//  SCK ( Serial clock ) :      		P9.30 pr1_pru0_pru_r30_2
//  Convst (  Start conversion ) : 	P9.31 pr1_pru0_pru_r30_0


/* includes */
#include <stdint.h>
#include <stdlib.h>
#include <pru_intc.h>
#include <pru_cfg.h>
#include "resource_table.h"

//Define pin locations
#define NRD 7
#define CS 5
#define MISO 3
#define MOSI 1
#define CLK 2
#define CONVST 0

/* GPIO registers */
volatile register uint32_t __R30;
volatile register uint32_t __R31;


void main(void) {
  uint8_t i;
    //  The data out line is connected to R30 bit 1.
  __R30 = 0x00000000;         //  Clear the output pin.
  __R31 = 0x00000000;		  //  Clear the input pin.
  __R30 |= (0 << CS);  // Initialize chip select LOW.
  __R30 |= (0 << NRD); // Initialize Read input LOW.
  __R30 |= (1 << CONVST); //Initialize conversion start HIGH.

  __R30 |= (0 << CLK); //Clock polarity 0
while (1){
  for (i = 0; i < 16; i++){ //Create a clock pulse for every received and send bit
    __R30 |= (1 << CLK);
    __delay_cycles(25);
    __R30 |= (0 << CLK);
  }
  __delay_cycles(2000000);
}


}
