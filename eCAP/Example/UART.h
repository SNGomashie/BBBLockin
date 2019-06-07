#ifndef __AM335X_PRU_UART_H_
#define __AM335X_PRU_UART_H_

#include <stdio.h>
#include <stdint.h>
#include <pru_uart.h>

#define FIFO_SIZE	16
#define MAX_CHARS	8
#define BUFFER		40

/**
 * Set the baudrate for UART communication
 *
 * @param baudrate     desired baudrate(1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200)
 * @return             void.
 */
void UART_set_baudrate(uint32_t baud_rate);


/**
 * Configure protocol for UART communication (Default 8, 0, 0, 0, 0, 0)
 *
 * @param wordlength  desired word length(5 bits, 6 bits, 7 bits, 8 bits)
 * @param parity      0 is parity disabled, 1 is parity enabled
 * @param select      0 is odd parity, 1 is even parity
 * @param stick       0 is stick parity disabled, 1 is stick parity enabled
 * @param break       0 is break condition disabled, 1 is break condition enabled
 * @param latch       0 is access to receive and transmit buffer, 1 is access to divisor registers
 * @return             void.
 */
void UART_set_protocol(uint8_t wordlength, uint8_t parity_en, uint8_t select_en, uint8_t stick_en, uint8_t break_en, uint8_t latch_en);

/**
 * Configure FIFO for UART communication (Default 1, 1, 1, 1, 2)
 *
 * @param fifo_en      Enable transmit and receive FIFOs
 * @param clearRX      0 has no effect, 1 clear receiver FIFO
 * @param clearTX      0 has no effect, 1 clear transmitter FIFO
 * @param DMA_mode     0 DMA MODE1 disabled, 1 DMA MODE0 enabled
 * @param trigger      Receiver FIFO trigger level(0 = 1 byte, 1 = 4 byes, 2 = 8 bytes, 3 = 14 bytes)
 * @return             void.
 */
void UART_set_FIFO(uint8_t fifo_en, uint8_t clearRX, uint8_t clearTX, uint8_t DMA_mode, uint8_t trigger);

/**
 * Configure autoflow and loopback for UART communication (Default 0, 0, 0)
 *
 * @param rts          0 RTS disabled; CTS enabled, 1 RTS & CTS enabled
 * @param loop         0 loopback mode is disabled, 1 loopback mode is enabled
 * @param auto         0 disables autoflow control, 1 enables autoflow control
 * @return             void.
 */
void UART_set_modem(uint8_t rts_en, uint8_t loop_en, uint8_t auto_en);

/**
 * Configure the power and emulation register (Default 1, 1, 1)
 *
 * @param on           Enable or Disable UART
 * @param rx           Enable RX
 * @param tx           Enable TX
 * @return             void.
 */
void UART_set_PEM(uint8_t on, uint8_t rx, uint8_t tx);

/**
 * Transmit a message to the UART module
 *
 * @param Message      Message to be send
 * @return             void.
 */
void UART_transmit(volatile char* Message);

/**
 * Transmit a message to the UART module
 *
 * @param void
 * @return *char       Returns a char array containing the message received
 */
char UART_receive(void);

#endif
