/**************************************/
/*        PRU PERIPHERAL LIBRARY      */
/*  An attempt at easier perhiperals  */
/*          By: Sena Gomashie         */
/**************************************/

#ifndef __PERIPHERAL_LIB_H_
#define __PERIPHERAL_LIB_H_

#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_intc.h>
#include <pru_iep.h>
#include <pru_ecap.h>
#include <sys_mcspi.h>
#include <pru_uart.h>


volatile register unsigned int __R30;
volatile register unsigned int __R31;

/*****************************/
/* PRU-ICSS Memory locations */
/*****************************/
#define SHARE_MEM  0x00010000  // 12kB of shared memory
volatile uint32_t *sMEM =  (unsigned int *) SHARE_MEM;

#define PRU0_MEM  0x00000000  // 8kB of Data RAM
volatile uint32_t *dMEM0 =  (unsigned int *) PRU0_MEM;

#define PRU1_MEM  0x00002000  // 8kB of Data RAM of the secondary PRU
volatile uint32_t *dMEM1 =  (unsigned int *) PRU1_MEM;

/* Power Reset Clock Module */
#define CM_PER_BASE	((volatile uint8_t *)(0x44E00000))
#define SPI0_CLKCTRL  (0x4C)
//TODO: Add more ass needed

void PRCMinitialize(void);

/* Interrupt controller addresses */
void INTCinitialize(uint8_t sys_evt, uint8_t chan, uint8_t host_int);
void INTCclear(uint8_t sys_evt);

/* Industrial Ethernet Peripheral */
typedef enum { cmp, cap } iep_config;

void IEPinitialize(uint32_t period, uint32_t increment, iep_config mode);
void IEPstart(void);
void IEPstop(void);
void IEPclear_int(void);

/* enhanced Capture module */
// volatile uint32_t *ecap_period = &CT_ECAP.CAP1;

void eCAPintialize(void);
void eCAPclear_int(void);

/* Multi-channel Serial Peripheral Interface */
// TODO: enum voor clock divider
void McSPIinitialze(uint8_t divider, uint8_t word_length, uint8_t ints);

/* Universal Asynchronous Receiver/Transmitter */
#define FIFO_SIZE	16
#define MAX_CHARS	8
#define BUFFER		40

void UARTinitialize(uint32_t baud_rate);
void UARTsend(volatile char* Message);
char UARTreceive(void);

/* Internal PRU-ICSS communication */
uint8_t INTERNCOM_status;

void INTERNCOMinitialize(uint8_t sys_evt, uint8_t channel, uint8_t host_int);

void INTERNCOMpoke(uint8_t int);

void INTERNCOMlisten(uint8_t pru, uint8_t int);
#endif /* __PERIPHERAL_LIB_H_ */
