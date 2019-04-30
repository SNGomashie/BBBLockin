/**************************************/
/*        PRU PERIPHERAL LIBRARY      */
/*  An attempt at easier perhiperals  */
/*          By: Sena Gomashie         */
/**************************************/

/**
 * @file "pru_peripheral.h"
 * @brief "The purpose of thise header file is to make interaction with the
 *         periherals using the PRU-ICSS easier"
 * @author "Sena Gomashie"
 * @date "23"/"April"/"2019"
 */

#ifndef __PERIPHERAL_LIB_H_
#define __PERIPHERAL_LIB_H_


/**
 *
 * @brief These header files can be found in the TI PRU software support package
 *
 * Find here:
 * @see https://git.ti.com/pru-software-support-package/pru-software-support-package/
 *
 */
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_intc.h>
#include <pru_iep.h>
#include <pru_ecap.h>
#include <sys_mcspi.h>
#include <pru_uart.h>

// PRU input / output registers
volatile register unsigned int __R30;
volatile register unsigned int __R31;

/****************************/
/* Power Reset Clock Module */
/****************************/

#define CM_PER_BASE	((volatile uint8_t *)(0x44E00000))
#define SPI0_CLKCTRL  (0x4C)
//TODO: Add more ass needed

/**
 * @brief Initialize Power Reser Clock module
 *
 * This function starts the module clock for selected peripherals
 */
void PRCMinitialize(void);



/**********************************/
/* Interrupt controller addresses */
/**********************************/

/**
 * Initialize PRU-ICSS interrupt controller
 *
 * @param sys_evt   System event to initialize
 * @param chan      Channel the system event should be connected to
 * @param host_int  Host interrupt the channel should be connected to
 *
 * @return          No return value
 *
 */
void INTCinitialize(uint8_t sys_evt, uint8_t chan, uint8_t host_int);


/**
 * Clear selected system event in the interrupt controller
 *
 * @param sys_evt   The system event to be cleared
 *
 * @return          No return value
 */
void INTCclear(uint8_t sys_evt);



/**********************************/
/* Industrial Ethernet Peripheral */
/**********************************/

// Timer modes
typedef enum {
  cmp,          /**< Compare mode. */
  cap           /**< Capture mode. */
} iep_config;

/**
 * Initialize timer module
 *
 * @param period        Define the timer frequency by period
 * @param increment     Increment by... on each clock cycle
 * @param mode          IEP modes: (cmp compares to value in ... reg)
 *                                 (cap captures the counter value at event)
 *
 * @return              No return value
 */
void IEPinitialize(uint32_t period, uint32_t increment, iep_config mode);

/**
 * Start the IEP timer
 *
 * @param void          No parameters requiered for this function
 *
 * @return void         No return value
 */
void IEPstart(void);

/**
 * Stop the IEP timer
 *
 * @param void          No parameters requiered for this function
 *
 * @return void         No return value
 */
void IEPstop(void);

/**
 * Clear the compare status register so system event 7 stops triggering
 *
 * @param void          No parameters requiered for this function
 *
 * @return void         No return value
 */
void IEPclear(void);



/***************************/
/* enhanced Capture module */
/***************************/

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

/**
 * @brief Initialize the enhanced Capture module
 *
 * Initializes the eCap module to capture the period of pin 42 and save
 * in the CMP1 register. Overwrites the CMP1 register on every event
 *
 * @param void          No parameters requiered for this function
 *
 * @return void         No return value
 */
void eCAPinitialize(void);


/**
 * @brief Clear the eCAP interrupt
 *
 * Clear the event status register so system event .. stops triggering
 *
 * @param void          No parameters requiered for this function
 *
 * @return void         No return value
 */
void eCAPclear_int(void);



/*********************************************/
/* Multi-channel Serial Peripheral Interface */
/*********************************************/

/**
 * @brief Initialize the McSPI module
 *
 * Initialiazes all the registers of the McSPI module for use with the PRU-ICSS
 *
 * @param divider          No parameters requiered for this function
 * @param word_length      Length of SPI word to be received / transmitted
 * @param int              Interrupts to be enabled
 * @return void            No return value
 */
void McSPIinitialze(uint8_t divider, uint8_t word_length, uint8_t ints);

void McSPIenable(uint8_t module);

void McSPIdisable(uint8_t module);


/***********************************************/
/* Universal Asynchronous Receiver/Transmitter */
/***********************************************/
#define FIFO_SIZE	16
#define MAX_CHARS	8
#define BUFFER		40

/**
 * @brief Initialiazes the UART module at a baud rate of 155200
 *
 * Initialiazes the UART module so you can use UART to easily debug your PRU program
 *
 * @param void          No parameters requiered for this function
 *
 * @return void         No return value
 */
void UARTinitialize(void);

/**
 * @brief Transmit a message using the UART module
 *
 * Sends a message to your PC using the UART module. You will need an FTDI cable for this.
 *
 * @param Message       The message you want to send as a pointer to the char array
 *
 * @return void         No return value
 */
void UARTtransmit(volatile char* Message);

/**
 * @brief Receive a message from the UART module
 *
 * Receive a message from the host PC
 *
 * @param void
 *
 * @return        Returns a char array of the received message
 */
char UARTreceive(void);



/*********************************************/
/* Internal PRU-ICSS communication */
/***********************************/
uint8_t INTERNCOM_status;

void INTERNCOMinitialize(uint8_t sys_evt, uint8_t channel, uint8_t host_int);

void INTERNCOMpoke(uint8_t inter);

void INTERNCOMlisten(uint8_t prus, uint8_t inter);



/****************************/
/* Instruction time counter */
/****************************/
void CYCLEstart(void);

uint32_t CYCLEstop(void);

#endif /* __PERIPHERAL_LIB_H_ */
