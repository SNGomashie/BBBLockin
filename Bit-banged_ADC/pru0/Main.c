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

//Define pin locations
#define NRD 7
#define CS 5
#define MISO 3
#define MOSI 1
#define CLK 2
#define CONVST 0

#include <stdint.h>
#include <pru_cfg.h>
#include "resource_table.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

uint8_t spiCommand = 0x00000000;
uint16_t spiReceive = 0x00000000;

#define PRU0_MEM 0x00000000
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

int8_t i;

uint16_t fnRead_WriteSPI(uint8_t chan);

void main(void)
{
	/* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	__R30 = 0x00000000;         //  Clear the output pin.
	__R31 = 0x00000000;		  //  Clear the input pin.
	__R30 |= (0 << CS);  // Initialize chip select LOW.
	__R30 |= (0 << NRD); // Initialize Read input LOW.
	__R30 |= (1 << CONVST); //Initialize conversion start HIGH.

		pru0_mem[5] = fnRead_WriteSPI(0);
		pru0_mem[6] = fnRead_WriteSPI(1);
		__halt();

}
uint16_t fnRead_WriteSPI(uint8_t chan){
	const uint8_t ADCch[] = {0, 4, 1, 5, 2, 6, 3, 7};

	spiCommand = ( ADCch[chan] << 4 ) | 0b10000000;	// single-ended, input +/-5V

	__R30 |= (1 << CS); //Set CS high
	__R30 |= (0 << NRD); //Set nRD low
	__R30 |= (0 << CONVST); //Set ConvST low
	__R30 |= (0 << CLK); //Set CLK low

	for (i = 0; i < 16; i++){ //Loop for every clock pulse
		spiReceive = spiReceive << 1; //shift

		if(spiCommand & (1 << i)) //write the command
		__R30 = ( 1 << MOSI );
		else
		__R30 = ( 0 << MOSI );

		__R30 = ( 1 << CLK ); //Rising edge Γ

		if (__R31 & ( 1 << MISO )) { //Save MISO
			spiReceive |= 0x1;
		} else {
			spiReceive |= 0x0;
		}
		__R30 = ( 0 << CLK ); //Falling edge Լ
	}

	__R30 |= ( 1 << NRD ); //Set nRD high
	__R30 |= ( 1 << CONVST ); //Set convST high
	__R30 |= ( 0 << CS ); //Set CS low

	return spiReceive;
}
