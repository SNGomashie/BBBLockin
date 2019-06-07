#ifndef __AM335X_PRU_IEP_H_
#define __AM335X_PRU_IEP_H_

#include <stdio.h>
#include <stdint.h>
#include <pru_iep.h>


/**
 * Reset the IEP module
 *
 * @param              void
 * @return             void.
 */
void IEP_reset(void);

/**
 * Select compare or capture mode
 *
 * @param mode         0 = Compare, 1 = Capture
 * @return             void.
 */
void IEP_mode(uint8_t mode);

/**
 * Select frequency, will compare against period(1/f) to generate event at this frequency
 *
 * @param freq         Frequency
 * @return             void.
 */
void IEP_frequency(uint32_t freq);

/**
 * Select period, will compare against this period to generate event at frequency 1/period
 *
 * @param period       period
 * @return             void.
 */
void IEP_period(uint32_t period);

/**
 * Select step size of counter
 *
 * @param increment    step size (Default = 5)
 * @return             void.
 */
void IEP_increment(uint16_t increment);

/**
 * Start IEP timer
 *
 * @param              void
 * @return             void.
 */
void IEP_start(void);

/**
 * Start IEP timer
 *
 * @param              void
 * @return             void.
 */
void IEP_stop(void);

/**
 * Clear IEP system event
 *
 * @param              void
 * @return             void.
 */
void IEP_clear(void);

#endif
