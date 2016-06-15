/*
 * globals.h
 *
 *  Created on: Jun 9, 2016
 *      Author: mcurtis
 */

#ifndef RFIDACCESSCONTROL_GLOBALS_H_
#define RFIDACCESSCONTROL_GLOBALS_H_

#define LED0 BIT0
#define LED1 BIT1
#define LED2 BIT2
#define LED3 BIT3
#define toggleLED(x) P1OUT ^= x
#define onLED(x) P1OUT |= x
#define offLED(x) P1OUT &= ~x
#define RXBUFFERLEN 80  //80 characters long
#define LCDADDR 0x27	//address of the LCD 4x20 display
#define I2CPRESCALER	10	//for i2c bus 10=100kHz @ smclk=1MHz

extern char rxbuffer[RXBUFFERLEN];
extern const char *prompt;
extern volatile unsigned int rxptr, rxwptr, sercmd;

/* Binary constant generator macro
   By Tom Torfs - donated to the public domain
*/
/* All macro's evaluate to compile-time constants */
/* *** helper macros *** */
/* turn a numeric literal into a hex constant
   (avoids problems with leading zeroes)
   8-bit constants max value 0x11111111, always fits in unsigned long
*/
#define HEX__(n) 0x##n##LU
/* 8-bit conversion function */
#define B8__(x) ((x&0x0000000FLU)?1:0) \
               +((x&0x000000F0LU)?2:0) \
               +((x&0x00000F00LU)?4:0) \
               +((x&0x0000F000LU)?8:0) \
               +((x&0x000F0000LU)?16:0) \
               +((x&0x00F00000LU)?32:0) \
               +((x&0x0F000000LU)?64:0) \
               +((x&0xF0000000LU)?128:0)
/* *** user macros *** */
/* for upto 8-bit binary constants */
#define B8(d) ((unsigned char)B8__(HEX__(d)))
/* for upto 16-bit binary constants, MSB first */
#define B16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8) \
   + B8(dlsb))
/* for upto 32-bit binary constants, MSB first */
#define B32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24)  \
      + ((unsigned long)B8(db2)<<16) \
      + ((unsigned long)B8(db3)<<8)  \
      + B8(dlsb))
/* Sample usage:
      B8(01010101) = 85
      B16(10101010,01010101) = 43605
      B32(10000000,11111111,10101010,01010101) = 2164238933
*/


#endif /* RFIDACCESSCONTROL_GLOBALS_H_ */
