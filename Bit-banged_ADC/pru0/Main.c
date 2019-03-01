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
#include <pru_cfg.h>
#include "resource_table.h"

//Define pin locations
#define NRD
#define CS 5
#define MISO 3
#define MOSI 1
#define CLK 2
#define CONVST 0

CT_CFG.GPCFG0 = 0;

/* GPIO registers */
volatile register uint32_t __R30;
volatile register uint32_t __R31;


void main(void) {
  volatile uint32_t clk;
  clk = 0x00000004;
  while(1) {
    __R30 ^= clk;
    __delay_cycles(20000000);
  }
}
