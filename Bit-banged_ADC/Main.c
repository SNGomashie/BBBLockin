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

/* PRU definiton */
#define PRU0

/* Data object to be send through the scratchpad */
typedef struct {
	uint16_t reg0;
	uint16_t reg1;
	uint16_t reg2;
	uint16_t reg3;
} bufferData;

bufferData dmemBuf;

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

/* Shared memory location & definiton*/
#define SHARE_MEM  0x00010000
#define PRU0_MEM 0x00000000
volatile uint32_t *shared =  (unsigned int *) SHARE_MEM;
volatile uint32_t *pru0_mem =  (unsigned int *) PRU0_MEM;

/* Interrupt definitions */
#define INT_OFF 0x00000000
#define INT_ON 0xFFFFFFFF

/* Number of sample channels */
const uint8_t Nchl = 4;
uint8_t idx;
uint8_t i;

/* SPI data */
uint16_t spiCommand = 0x00000000;
uint16_t spiReceive = 0x00000000;

uint16_t fnRead_WriteSPI(uint8_t chan);

void main(void) {
	shared[0] = INT_OFF;
	/* 1.	Initialization */
                              //  The data out line is connected to R30 bit 1.
	__R30 = 0x00000000;         //  Clear the output pin.
	__R31 = 0x00000000;		  //  Clear the input pin.
	__R30 |= (0 << CS);  // Initialize chip select LOW.
	__R30 |= (0 << NRD); // Initialize Read input LOW.
	__R30 |= (1 << CONVST); //Initialize conversion start HIGH.

/* Infinite loop */
	while(1) {
		while(shared[0] == INT_OFF){
			/* Fill the struct with 16 bit random values */
			dmemBuf.reg0 = fnRead_WriteSPI(0);
			dmemBuf.reg1 = fnRead_WriteSPI(1);
			dmemBuf.reg2 = fnRead_WriteSPI(2);
			dmemBuf.reg3 = fnRead_WriteSPI(3);

			/* Send interrupt over shared memory */
			shared[0] = INT_ON;

			/* Send data object through the scratchpad */
			__xout(10, 0, 0, dmemBuf);

			/* Delay for a second */
			__delay_cycles(200000000);
		}
	}
}

uint16_t fnRead_WriteSPI(uint8_t chan){
const uint8_t ADCch[] = {0, 4, 1, 5, 2, 6, 3, 7};

spiCommand = ( ADCch[chan] << 4 ) | 0b10000000;	// single-ended, input +/-5V

__R30 |= (1 << CS); //Set CS high
__R30 |= (0 << NRD); //Set nRD low
__R30 |= (0 << CONVST); //Set ConvST low

__R30 |= (0 << CLK);

for (i = 0; i < 16; i++) { //Create a clock pulse for every received and send bit
	spiReceive = spiReceive << 1; //shift

	if (( spiCommand << i ) & 0x80){ //
		__R30 |= ( 1 << MOSI );
	} else {
		__R30 &= ~( 1 << MOSI );
	}

	__R30 |= ( 1 << CLK ); //Rising edge Γ

	if (__R31 & ( 1 << MISO )) {
		spiReceive |= 0x01;
	} else {
		spiReceive &= ~(0x01);
	}

	__R30 |= ~( 1 << CLK ); //Falling edge Լ
}

__R30 |= ( 1 << NRD );
__R30 |= ( 1 << CONVST );
__R30 |= ( 0 << CS );

return spiReceive;
}