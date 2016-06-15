/*
 * spi.h
 *
 *  Created on: Jun 9, 2016
 *      Author: mcurtis
 */

#ifndef RFIDACCESSCONTROL_SPI_H_
#define RFIDACCESSCONTROL_SPI_H_

#define RC522CS		BIT4 //on input 5.4
#define RC522RST	BIT5 //on input 5.4

void spisetup(unsigned char prescaler, unsigned char enablepin);
void spitxm(int enpin, unsigned char *data, unsigned int len);
void spirxm(int enpin, unsigned char addr, unsigned char *data, unsigned int len);//BROKEN
void spitx(int enpin, unsigned char data);
void spitxx(int enpin, unsigned char addr, unsigned char data);
void spirx(int enpin, unsigned char addr, unsigned char *data);
void spitxrx(int enpin, unsigned char addr, unsigned char *data, unsigned int len, unsigned char inc);

#endif /* RFIDACCESSCONTROL_SPI_H_ */
