/**************************************/
/*        PRU PERIPHERAL LIBRARY      */
/*  An attempt at perhiperals easier  */
/*          By: Sena Gomashie         */
/**************************************/

#ifndef __PERIPHERAL_LIB_C_
#define __PERIPHERAL_LIB_C_

#include <stdint.h>
#include <stdio.h>
#include "pru_peripheral.h"


/****************************/
/* Power Reset Clock Module */
/****************************/
void PRCMinitialize(void){
  /* Intialize pointer variable */
  volatile uint8_t *ptr_cm;

  /* point to control module base address */
  ptr_cm = CM_PER_BASE;

  /* Activate SPI0 clock */
  ptr_cm[SPI0_CLKCTRL] = 0x2;
  // Look up more
}

/**********************************/
/*      Interrupt controller      */
/**********************************/
void INTCinitialize(uint8_t sys_evt, uint8_t chan, uint8_t host_int){
  // Disable global interrupts
  CT_INTC.GER = 0;

  // Clear current interrupts
  __R31 = 0x00000000;

  // Configure INTC
  if (sys_evt < 4) {
    CT_INTC.CMR0 = (chan << (sys_evt * 8));
  } else if (sys_evt < 8) {
    CT_INTC.CMR1 = (chan << ((sys_evt * 8) - 32));
  } else if (sys_evt < 12) {
    CT_INTC.CMR2 = (chan << ((sys_evt * 8) - 64));
  } else if (sys_evt < 16) {
    CT_INTC.CMR3 = (chan << ((sys_evt * 8) - 96));
  } else if (sys_evt < 20) {
    CT_INTC.CMR4 = (chan << ((sys_evt * 8) - 128));
  } else if (sys_evt < 24) {
    CT_INTC.CMR5 = (chan << ((sys_evt * 8) - 160));
  } else if (sys_evt < 28) {
    CT_INTC.CMR6 = (chan << ((sys_evt * 8) - 192));
  } else if (sys_evt < 32) {
    CT_INTC.CMR7 = (chan << ((sys_evt * 8) - 224));
  } else if (sys_evt < 36) {
    CT_INTC.CMR8 = (chan << ((sys_evt * 8) - 256));
  } else if (sys_evt < 40) {
    CT_INTC.CMR9 = (chan << ((sys_evt * 8) - 288));
  } else if (sys_evt < 44) {
    CT_INTC.CMR10 = (chan << ((sys_evt * 8) - 320));
  } else if (sys_evt < 48) {
    CT_INTC.CMR11 = (chan << ((sys_evt * 8) - 352));
  } else if (sys_evt < 52) {
    CT_INTC.CMR12 = (chan << ((sys_evt * 8) - 384));
  } else if (sys_evt < 56) {
    CT_INTC.CMR13 = (chan << ((sys_evt * 8) - 416));
  } else if (sys_evt < 60) {
    CT_INTC.CMR14 = (chan << ((sys_evt * 8) - 448));
  } else if (sys_evt < 64) {
    CT_INTC.CMR15 = (chan << ((sys_evt * 8) - 480));
  }

  if (chan < 4) {
    CT_INTC.HMR0 = (host_int << (chan * 8));
  } else if (chan < 8) {
    CT_INTC.HMR1 = (host_int << ((chan * 8) - 32));
  } else if (chan < 12) {
    CT_INTC.HMR2 = (host_int << ((chan * 8) - 64));
  }

  CT_INTC.SICR = sys_evt;
  CT_INTC.EISR = sys_evt;
  CT_INTC.HIEISR = host_int;

  // Enable global interrupts
  CT_INTC.GER = 1;
}

void INTCclear(uint8_t sys_evt){
  CT_INTC.SICR = sys_evt;
  __delay_cycles(5);
}

/**********************************/
/* Industrial Ethernet Peripheral */
/**********************************/
void IEPinitialize(uint32_t period, uint32_t increment, iep_config mode){

  /* Disable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0000;

  /* Clear CNT register */
  CT_IEP.TMR_CNT = 0xFFFFFFFF;

  /* Clear overflow register */
  CT_IEP.TMR_GLB_STS = 0x0001;

  /* Clear compare status */
  CT_IEP.TMR_CMP_STS = 0x0001;

  if(mode == cmp){
    /* Set compare values */
    CT_IEP.TMR_CMP0 = period;

    /* Enable compare event */
    CT_IEP.TMR_CMP_CFG = 0x0003;
  }
  /* Set increment to 1 (default = 5)*/
  CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = increment;
}

void IEPstart(void){
  /* Enable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0001;
}

void IEPstop(void){
  /* Disable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0x0000;
}

void IEPclear_int(void){
  /* Clear Compare status */
  CT_IEP.TMR_CMP_STS = (1 << 0);

  /* delay for 5 cycles, clearing takes time */
  __delay_cycles(5);
}

/****************************/
/* Enchanced Capture Module */
/****************************/
void eCAPinitialize(void){
	CT_ECAP.ECCTL2 &= ~(1 << 4);
	/* Difference mode */
	CT_ECAP.ECCTL1 |= (1 << 1);

	/* Enable loading of CAP registers */
	CT_ECAP.ECCTL1 |= (1 << 8);

	CT_ECAP.ECCTL2 |= (0x2 << 6);

	CT_ECAP.ECCTL2 |= (1 << 4);
}

/*********************************************/
/* Multi-channel Serial Peripheral Interface */
/*********************************************/
void McSPIinitialze(uint8_t divider, uint8_t word_length, uint8_t ints){
  /* Reset McSPI0 module */
  CT_MCSPI0.SYSCONFIG_bit.SOFTRESET = 0x0001;

  /* Wait until reset is done */
  while(!(CT_MCSPI0.SYSSTATUS_bit.RESETDONE == 0x1));

  /* Reset interrupt status */
  CT_MCSPI0.IRQSTATUS = 0xFFFF;

  /* Configure interrupts */
  CT_MCSPI0.IRQENABLE = 0x0;

  // Set clock devider, SPI clock = 48MHz, Device clock = 20Mhz. devider = 4;
  CT_MCSPI0.CH0CONF_bit.CLKD = 0x2;

  /* Set world length to 16bit */
  CT_MCSPI0.CH0CONF_bit.WL = 0xF;

  // Set SPID0 as not a transmissionline
  CT_MCSPI0.CH0CONF_bit.DPE0 = 0x1;

  // Set SPID0 as input
  CT_MCSPI0.CH0CONF_bit.IS = 0x0;
}

/**********************************/
/*  Universal Asynchronous Receiver/Transmitter  */
/* 1200, 2400, 4800, 19200, 38400, 57600, 115200 */
/*************************************************/
void UARTinitialize(uint32_t baud_rate){
  /* Verify acceptable input */
  while(!((baud_rate == 1200) || (baud_rate == 2400) || (baud_rate == 4800) || (baud_rate == 19200) || (baud_rate == 38400) || (baud_rate == 57600) || !(baud_rate == 115200)));
  baud_rate /= 100;
  baud_rate = 1920000 / baud_rate;
  baud_rate /= 16;

  /* Configure baudrate */
  CT_UART.DLL = (0xFF) & baud_rate;
  CT_UART.DLH = (0xFF00) & baud_rate;
  CT_UART.MDR_bit.OSM_SEL = 0x0;

  /* Set up UART to function at 115200 baud - DLL divisor is 104 at 16x oversample
  * 192MHz / 104 / 16 = ~115200 */
  // CT_UART.DLL = 104;
  // CT_UART.DLH = 0;
  // CT_UART.MDR_bit.OSM_SEL = 0x0;

  /* If FIFOs are to be used, select desired trigger level and enable
  * FIFOs by writing to FCR. FIFOEN bit in FCR must be set first before
  * other bits are configured */
  /* Enable FIFOs for now at 1-byte, and flush them */
  CT_UART.FCR = (0x80) | (0x8) | (0x4) | (0x2) | (0x01); // 8-byte RX FIFO trigger (0x8f)

  /* Choose desired protocol settings by writing to LCR */
  /* 8-bit word, 1 stop bit, no parity, no break control and no divisor latch */
  CT_UART.LCR = 3;
  CT_UART.LCR_bit.DLAB = 0;
  /* If flow control is desired write appropriate values to MCR. */
  /* No flow control for now, but enable loopback for test */
  CT_UART.MCR = 0x00;

  /* Choose desired response to emulation suspend events by configuring
  * FREE bit and enable UART by setting UTRST and URRST in PWREMU_MGMT */
  /* Allow UART to run free, enable UART TX/RX */
  CT_UART.PWREMU_MGMT_bit.FREE = 0x1;
  CT_UART.PWREMU_MGMT_bit.URRST = 0x1;
  CT_UART.PWREMU_MGMT_bit.UTRST = 0x1;

  /* Turn off RTS and CTS functionality */
  CT_UART.MCR_bit.AFE = 0x0;
  CT_UART.MCR_bit.RTS = 0x0;
}

void UARTtransmit(volatile char* Message){
  uint8_t cnt, index = 0;

  while (1) {
    cnt = 0;

    /* Wait until the TX FIFO and the TX SR are completely empty */
    while (!CT_UART.LSR_bit.TEMT);

    /* Loop aslong as there are new characters in Message and cnt is not bigger than MAX_CHARS*/
    while (Message[index] != NULL && cnt < MAX_CHARS) {
      /* Fill TX FIFO with 1 char */
      CT_UART.THR = Message[index];
      index++;
      cnt++;
    }
    /* if there are no new characters in Message break out of the while(1) loop */
    if (Message[index] == NULL)
    break;
  }

  /* Wait until the TX FIFO and the TX SR are completely empty */
  while (!CT_UART.LSR_bit.TEMT);
}

char UARTreceive(void){
  while (!CT_UART.LSR_bit.DR);

  return CT_UART.RBR_bit.DATA;
}

/***********************************/
/* Internal PRU-ICSS communication */
/***********************************/
void INTERNCOMinitialize(uint8_t sys_evt){
  INTCinitialize(sys_evt, 0, 0);
  INTERNCOM_status = 1;
}

void INTERNCOMtransmit(uint8_t device_id, uint32_t base_register, uint16_t object){
  if(INTERNCOM_status == 1){
    __R31 |= (1 << 30);
    __xout(14, 0, 0, object);
  }
}

void INTERNCOMreceive(uint8_t device_id, uint32_t base_register, uint16_t object){
  if(INTERNCOM_status == 1){
    while(!(__R31 & (1 << 30)));
      __xin(14, 0, 0, object);
      INTCclear(20);
  }
}

void INTERNCOMpoke(void){
  if(INTERNCOM_status == 1){
    __R31 |= (1 << 30);
  }
}

void INTERNCOMlisten(void){
  if(INTERNCOM_status == 1){
    while(!(__R31 & (1 << 30)));
  }
}


#endif