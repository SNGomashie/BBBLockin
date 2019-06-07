#ifndef __AM335X_PRU_GPIO_H_
#define __AM335X_PRU_GPIO_H_

#include <stdio.h>
#include <stdint.h>
#include <pru_cfg.h>

#define GPIO1 0x4804C000  // external GPIO register
#define GPIO_CLEARDATAOUT 0x190  // Value for clearing output
#define GPIO_SETDATAOUT 0x194  // Value for setting output
#define USR0 (1<<21)  // Adress of LED0
#define USR1 (1<<22)  // Adress of LED1
#define USR2 (1<<23)  // Adress of LED2
#define USR3 (1<<24)  // Adress of LED3  (Closest to ethernet port)

volatile register unsigned int __R30;  // Output register
volatile register unsigned int __R31;  // Input register

/**
 * Turn on LED, name specified in parameter.
 *
 * @param led          name of LED
 * @return             true on success, false if an error occured.
 */
void PRU_LED_SET(uint32_t led);

/**
 * Turn off LED, name specified in parameter.
 *
 * @param led          name of LED
 * @return             true on success, false if an error occured.
 */
void PRU_LED_CLEAR(uint32_t led);

/**
 * Toggle LED, number specified in parameter.
 *
 * @param led          address of LED
 * @return             true on success, false if an error occured.
 */
void PRU_LED_TOGGLE(uint32_t led);


/**
 * Set output pin, pin number specified in parameter.
 *
 * @param pin          Pin number
 * @return             true on success, false if an error occured.
 */
void PRU_GPO_SET(uint8_t pin);

/**
 * Clear output pin, pin number specified in parameter.
 *
 * @param pin          Pin number
 * @return             true on success, false if an error occured.
 */
void PRU_GPO_CLEAR(uint8_t pin);

/**
 * Toggle output pin, pin number specified in parameter.
 *
 * @param pin          Pin number
 * @return             true on success, false if an error occured.
 */
void PRU_GPO_TOGGLE(uint8_t pin);


enum GPO_mode{direct_out,   // Direct output mode
              serial};  // Serial output mode

enum GPI_mode{direct_in,   // Direct input mode
              parallel, // Parallel input mode
              shift,    // 28-bit shift in mode
              mii_rt};  // MII_RT mode

enum GPI_clk{pos,       // capture on the positive edge
             neg};      // capture on the negative edge
/**
 * Select what shadow register to use for GPO shifting.
 *
 * @param core         core to configure
 * @param pin          reg number (0 or 1)
 * @return             true on success, false if an error occured.
 */
void PRU_GPO_SH_SEL(uint8_t core, uint8_t reg);

/**
 * Shift out rate is decided by 2 cascaded divisors. These are applied to ocp_clk of 200Mhz
 *
 * @param core         core to configure
 * @param divisor      second divisor
 * @return             true on success, false if an error occured.
 */
void PRU_GPU_DIV1(uint8_t core, uint8_t divisor);

/**
 * Shift out rate is decided by 2 cascaded divisors. These are applied to ocp_clk of 200Mhz
 *
 * @param core         core to configure
 * @param divisor      first divisor
 * @return             true on success, false if an error occured.
 */
void PRU_GPO_DIV0(uint8_t core, uint8_t divisor);

/**
 * Select the output mode of the selected PRU
 *
 * @param core         core to configure
 * @param mode         direct or serial output mode
 * @return             true on success, false if an error occured.
 */
void PRU_GPO_MODE(uint8_t core, enum GPO_mode mode);


/**
 * Regiser is set when first capture happend in 28-bit shift mode
 *
 * @param core         core to configure
 * @param clear        writing 1 will clear start bit event for 28-bit shift in mode
 * @return             register value
 */
uint8_t PRU_GPI_SB(uint8_t core, uint8_t clear);

/**
 * Shift in rate is decided by 2 cascaded divisors. These are applied to ocp_clk of 200Mhz
 *
 * @param core         core to configure
 * @param pin          second divisor
 * @return             true on success, false if an error occured.
 */
void PRU_GPI_DIV1(uint8_t core, uint8_t divisor);

/**
 * Shift in rate is decided by 2 cascaded divisors. These are applied to ocp_clk of 200Mhz
 *
 * @param core         core to configure
 * @param divisor      first divisor
 * @return             true on success, false if an error occured.
 */
void PRU_GPI_DIV0(uint8_t core, uint8_t divisor);

/**
 * Select block edge for parralel 16-bit capture mode
 *
 * @param core         core to configure
 * @param mode         pos for positive edge, neg for negative edge
 * @return             true on success, false if an error occured.
 */
void PRU_GPI_CLK(uint8_t core, enum GPI_clk mode);

/**
 * Select input mode of the selected PRU
 *
 * @param core         core to configure
 * @param mode         direct, parrallel, shift or Mii_rt mode
 * @return             true on success, false if an error occured.
 */
void PRU_GPI_MODE(uint8_t core, enum GPI_mode mode);


#endif
