#ifndef __AM335X_PRU_GPIO_C_
#define __AM335X_PRU_GPIO_C_

#include "GPIO.h"

unsigned int volatile * const GPIO1_CLEAR = (unsigned int *) (GPIO1 + GPIO_CLEARDATAOUT);
unsigned int volatile * const GPIO1_SET   = (unsigned int *) (GPIO1 + GPIO_SETDATAOUT);

/* On-board LEDS */
void PRU_LED_SET(uint32_t led){
  *GPIO1_SET = led;
}

void PRU_LED_CLEAR(uint32_t led){
  *GPIO1_CLEAR = led;
}



/* Input & output pins */
void PRU_GPO_SET(uint8_t pin){
  __R30 |= (1 << pin);
}

void PRU_GPO_CLEAR(uint8_t pin){
  __R30 &= ~(1 << pin);
}

void PRU_GPO_TOGGLE(uint8_t pin){
  __R30 ^= (1 << pin);
}


/* Input & output registers */
void PRU_GPO_SH_SEL(uint8_t core, uint8_t reg){
  if(core == 0){
    CT_CFG.GPCFG0_bit.PRU0_GPO_SH_SEL = reg;
  } else{
    CT_CFG.GPCFG1_bit.PRU1_GPO_SH_SEL = reg;
  }
}

void PRU_GPU_DIV1(uint8_t core, uint8_t divisor){
  if(core == 0){
    CT_CFG.GPCFG0_bit.PRU0_GPO_DIV1 = divisor;
  } else{
    CT_CFG.GPCFG1_bit.PRU1_GPO_DIV1 = divisor;
  }
}

void PRU_GPO_DIV0(uint8_t core, uint8_t divisor){
  if(core == 0){
    CT_CFG.GPCFG0_bit.PRU0_GPO_DIV0 = divisor;
  } else{
    CT_CFG.GPCFG1_bit.PRU1_GPO_DIV0 = divisor;
  }
}

void PRU_GPO_MODE(uint8_t core, enum GPO_mode mode){
  if(core == 0){
    CT_CFG.GPCFG0_bit.PRU0_GPO_MODE = mode;
  } else{
    CT_CFG.GPCFG1_bit.PRU1_GPO_MODE = mode;
  }
}

uint8_t PRU_GPI_SB(uint8_t core, uint8_t clear){
  if(core == 0){
    if(clear == 1){
      CT_CFG.GPCFG0_bit.PRU0_GPI_SB = clear;
      return 0;
    } else {
      return CT_CFG.GPCFG0_bit.PRU0_GPI_SB;
    }
  } else{
    if(clear == 1){
      CT_CFG.GPCFG1_bit.PRU1_GPI_SB = clear;
      return 0;
    } else {
      return CT_CFG.GPCFG1_bit.PRU1_GPI_SB;
    }
  }
}

void PRU_GPI_DIV1(uint8_t core, uint8_t divisor){
  if(core == 0){
    CT_CFG.GPCFG0_bit.PRU0_GPI_DIV1 = divisor;
  } else{
    CT_CFG.GPCFG1_bit.PRU1_GPI_DIV1 = divisor;
  }
}

void PRU_GPI_DIV0(uint8_t core, uint8_t divisor){
  {
    if(core == 0){
      CT_CFG.GPCFG0_bit.PRU0_GPI_DIV0 = divisor;
    } else{
      CT_CFG.GPCFG1_bit.PRU1_GPI_DIV0 = divisor;
    }
  }
}

void PRU_GPI_CLK(uint8_t core, enum GPI_clk mode){
  if(core == 0){
    CT_CFG.GPCFG0_bit.PRU0_GPI_CLK_MODE = mode;
  } else{
    CT_CFG.GPCFG1_bit.PRU1_GPI_CLK_MODE = mode;
  }
}

void PRU_GPI_MODE(uint8_t core, enum GPI_mode mode){
  if(core == 0){
    CT_CFG.GPCFG0_bit.PRU0_GPI_MODE = mode;
  } else{
    CT_CFG.GPCFG1_bit.PRU1_GPI_MODE = mode;
  }
}


#endif
