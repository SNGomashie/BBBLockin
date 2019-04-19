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
void eCAPinitialize(void);

void eCAPclear_int(void);


/* Multi-channel Serial Peripheral Interface */
// TODO: enum voor clock divider
void McSPIinitialze(uint8_t divider, uint8_t word_length, uint8_t ints);

/* Universal Asynchronous Receiver/Transmitter */
#define FIFO_SIZE	16
#define MAX_CHARS	8
#define BUFFER		40

void UARTinitialize(void);

void UARTtransmit(volatile char* Message);

char UARTreceive(void);


/* Internal PRU-ICSS communication */
uint8_t INTERNCOM_status;

void INTERNCOMinitialize(uint8_t sys_evt);

void INTERNCOMtransmit(uint8_t device_id, uint32_t base_register, uint16_t object);

void INTERNCOMreceive(uint8_t device_id, uint32_t base_register, uint16_t object);

void INTERNCOMpoke(void);

void INTERNCOMlisten(void);


/* Instruction time counter */
void CYCLEstart(void);

uint32_t CYCLEstop(void);
#endif /* __PERIPHERAL_LIB_H_ */
