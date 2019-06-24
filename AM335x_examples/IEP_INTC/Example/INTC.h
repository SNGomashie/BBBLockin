#ifndef __AM335X_PRU_INTC_H_
#define __AM335X_PRU_INTC_H_

#include <stdio.h>
#include <stdint.h>
#include <pru_intc.h>
#include "GPIO.h"

/**
 * Connects specified system event to specified channel to specified host interrupt
 *
 * @param sys_evt      System event to be configured
 * @param chan         Channel to connect the system event to
 * @param host_int     Host interrupt to connect channel to
 * @return             void
 */
void INTC_initialize(uint8_t sys_evt, uint8_t chan, uint8_t host_int);

/**
 * Clear an system event, clearing takes a couple cycles. To prevent double activation we wait a few cycles
 *
 * @param sys_evt      System event to be clear
 * @return             void
 */
void INTC_clear(uint8_t sys_evt);

/**
 * Toggle an system event to poke the other PRU or an external system
 *
 * @param inter        system event to be activated
 * @return             void
 */
void INTC_poke(uint8_t inter);

/**
 * Poll host interrupt 0 for PRU0 or host interrupt 1 for PRU1.
 *
 * @param prus         PRU to listen to
 * @param inter        Clear the interrupt used to poke
 * @return             void
 */
void INTC_listen(uint8_t prus, uint8_t inter);

#endif
