/*
 * i2c.h
 *
 *  Created on: Jun 9, 2016
 *      Author: mcurtis
 */

#ifndef RFIDACCESSCONTROL_I2C_H_
#define RFIDACCESSCONTROL_I2C_H_

void TI_USCI_I2C_receiveinit(unsigned char slave_address, unsigned char prescale);
void TI_USCI_I2C_transmitinit(unsigned char slave_address, unsigned char prescale);
void TI_USCI_I2C_receive(unsigned char byteCount, unsigned char *field);
void TI_USCI_I2C_transmit(unsigned char byteCount, unsigned char *field);
unsigned char TI_USCI_I2C_slave_present(unsigned char slave_address);
unsigned char TI_USCI_I2C_notready();

void i2cinit(unsigned int prescaler);
void i2ctx(unsigned char addr, unsigned char *data, unsigned int len);
void i2crx(unsigned char addr, unsigned char *data, unsigned int len);
void i2ctxrx(unsigned char addr, unsigned char txdata ,unsigned char *data, unsigned int len);
unsigned char i2cslavecheck(unsigned char slave_address);

#define SDA_PIN 0x02                                  // msp430x261x UCB0SDA pin
#define SCL_PIN 0x04                                  // msp430x261x UCB0SCL pin

#endif /* RFIDACCESSCONTROL_I2C_H_ */
