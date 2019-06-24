#ifndef __AM335X_PRU_ECAP_H_
#define __AM335X_PRU_ECAP_H_

#include <stdio.h>
#include <stdint.h>
#include <pru_ecap.h>

// ECCTL1 register
#define CAP1POL 0
#define CTRRST1 1
#define CAP2POL 2
#define CTRRST2 3
#define CAP3POL 4
#define CTRRST3 5
#define CAP4POL 6
#define CTRRST4 7
#define CAPLDEN 8
#define PRESCALE 9
#define FREE_SOFT 14

// ECCTL2 register
#define CONT_ONESHT 0
#define	STOP_WRAP 1
#define RE_ARM 3
#define TSCTRSTOP 4
#define SYNCI_EN 5
#define SYNCO_SEL 6
#define SWSYNC 8
#define CAP_APWM 9
#define APWMPOL 10

static volatile uint32_t *ptrECAP = &CT_ECAP.CAP1;

/**
 * Choose the polarity of a capture event (default 0, 0)
 *
 * @param event        Choose capture event [0:3]
 * @param polarity     0 = rising edge, 1 = falling edge
 * @return             void
 */
void eCAP_polarity(uint8_t event, uint8_t polarity);

/**
 * Choose how the counter should respond to an event.
 *
 * @param reg          Choose capture event [0:3]
 * @param mode         0 = no reset on event (absolute timestamp), 1 = reset on event (difference mode)
 * @return             void
 */
void eCAP_reset(uint8_t event, uint8_t mode);

/**
 * Enable the loading of CAP registers on an event
 *
 * @param load         0 = Disable loading, 1 = Enable loading of capture registers on event
 * @return             void
 */
void eCAP_load(uint8_t load);

/**
 * Define the event prescaler.
 *
 * @param scaler       Prescaler value
 * @return             void
 */
void eCAP_prescaler(uint8_t scaler);

/**
 * Chooses if the module is in capture mode or PWM mode
 *
 * @param mode         0 = Capture mode, 1 = PWM mode
 * @return             void
 */
void eCAP_mode(uint8_t mode);

/**
 * Chooses if the module is in continuous mode or one-shot mode
 *
 * @param mode         0 = Continuous mode, 1 = One-shot mode
 * @return             void
 */
void eCAP_continuous(uint8_t mode);

/**
 * Choose how the sync-out/sync-in is to be used
 *
 * @param out          0 = Sync in is sync out, 1 = PRDEQ is sync out, 2&3 is disable sync out
 * @param in           0 = Disable sync-in, 1 = Enable sync-in
 * @return             void
 */
void eCAP_sync(uint8_t out, uint8_t in);

/**
 * One-shot: Reg after which to stop
 * Continuous: Number of captures before wrap back to first register
 *
 * @param reg         number corresponds to the reg you want to wrap at
 * @return             void
 */
void eCAP_wrap(uint8_t reg);

/**
 * Start the eCAP module
 *
 * @param              void
 * @return             void
 */
void eCAP_start(void);

/**
 * Stop the eCAP module
 *
 * @param              void
 * @return             void
 */
void eCAP_stop(void);

#endif
