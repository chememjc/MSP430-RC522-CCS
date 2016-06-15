/*
 * serial.h
 *
 *  Created on: Jun 9, 2016
 *      Author: mcurtis
 */

#ifndef RFIDACCESSCONTROL_RS232SHELL_H_
#define RFIDACCESSCONTROL_RS232SHELL_H_

//serial ISR cases
#define RXEVAL	BIT0 //evaluate command buffer
#define EBUFF	BIT1 //buffer overflow error
#define TXNL	BIT2 //transmit new line
#define TXBS	BIT3 //transmit backspace
#define TXCTLC	BIT4 //transmit Ctl+C

extern volatile unsigned int rxptr, rxwptr, sercmd; //defined in main.c

void serout(const char *str);
void handleinput();

#endif /* RFIDACCESSCONTROL_RS232SHELL_H_ */
