/*
 * spi.c
 *
 *  Created on: Jun 9, 2016
 *      Author: mcurtis
 */
#include <msp430.h>
#include <msp430f2419.h>
#include "globals.h"
#include "spi.h"

void spisetup(unsigned char prescaler, unsigned char enablepin){
	/*//bring RC522 out of reset
	P5OUT |= RC522RST;*/
	P5OUT |= enablepin;  //deselect chip (mask)
	//Set up SPI
	UCB1CTL1 =  UCSWRST; //put USCI in reset while it is configured
	UCB1CTL0 =  UCMST + UCMSB + UCSYNC +UCCKPH; //put UCMODE as 00 for 3 wire SPI w/ MSB first!
	UCB1CTL1 =  UCSSEL1; //SMCLK
	UCB1BR0  =  prescaler;
	UCB1BR1  =  0;
	UCB1STAT =  0;
	UCB1CTL1 &= ~UCSWRST; //take USCI out of reset; ready to go!
	return;
}

void spitxm(int enpin, unsigned char *data, unsigned int len){ //BROKEN!!
	unsigned char dummy;
	unsigned int i=0;
	//while(!(UC1IFG & UCB1TXIFG));//make sure the bus is not in use
	while(UCB1STAT & UCBUSY);//make sure the bus is not in use
	if(enpin>=0)P5OUT &= ~enpin; //set CS low to select chip
	while(len-i){
		while(!(UC1IFG & UCB1TXIFG));
		dummy=UCB1RXBUF; //clear the rxready flag
		UCB1TXBUF=data[i];
		i++;
	}
	if(enpin>=0){
		while(UCB1STAT & UCBUSY);//wait for byte to be fully sent
		P5OUT |= enpin; //set CS high to deselect chip iff enpin is positive
	}
	dummy++;
	return;
}
void spirxm(int enpin, unsigned char addr, unsigned char *data, unsigned int len){ //BROKEN!!
	unsigned int i=0;
	while(UCB1STAT & UCBUSY);//make sure the bus is not in use
	if(enpin>=0) P5OUT &= ~enpin; //set CS low to select chip
	while(len-i){
		UCB1TXBUF=addr;
		while(!(UC1IFG & UCB1TXIFG));//wait for byte to be sent
		data[i]=UCB1RXBUF;
		i++;
	}
	if(enpin>=0) {
		while(UCB1STAT & UCBUSY); //wait for last byte to be sent
		P5OUT |= enpin; //set CS high to deselect chip
	}
	return;
}

void spitx(int enpin, unsigned char data){
	unsigned char dummy;
	while(UCB1STAT & UCBUSY);//make sure the bus is not in use iff enpin is positive
	if(enpin>=0)P5OUT &= ~enpin; //set CS low to select chip
	dummy=UCB1RXBUF; //clear the rxready flag
	UCB1TXBUF=data;
	if(enpin>=0){
		while(UCB1STAT & UCBUSY);//wait for byte to be fully sent
		P5OUT |= enpin; //set CS high to deselect chip iff enpin is positive
	}
	dummy++;
	return;
}
void spirx(int enpin, unsigned char addr, unsigned char *data){
	unsigned char dummy;
	while(UCB1STAT & UCBUSY);//make sure the bus is not in use
	if(enpin>=0) P5OUT &= ~enpin; //set CS low to select chip
	dummy=UCB1RXBUF; //clear the rxready flag
	UCB1TXBUF=addr;
	while(!(UC1IFG & UCB1RXIFG));//wait for byte to be received
	*data=UCB1RXBUF;
	if(enpin>=0) P5OUT |= enpin; //set CS high to deselect chip
	dummy++;
	return;
}

void spitxrx(int enpin, unsigned char addr, unsigned char *data, unsigned int len, unsigned char inc){
	unsigned int i=0;
	unsigned char workingaddr=addr, dummy;
	while(UCB1STAT & UCBUSY);//make sure the bus is not in use
	P5OUT &= ~enpin; //set CS low to select chip
	dummy=UCB1RXBUF; //clear the rxready flag
	UCB1TXBUF=addr;
	while(!(UC1IFG & UCB1RXIFG));//wait for byte to be received
	data[0]=UCB1RXBUF;
	while(len-i){
		workingaddr += inc;
		UCB1TXBUF=workingaddr;
		while(!(UC1IFG & UCB1RXIFG));//wait for byte to be received
		data[i]=UCB1RXBUF;
		i++;
	}
	P5OUT |= enpin; //set CS high to deselect chip
	dummy++;
	return;
}

void spitxx(int enpin, unsigned char addr, unsigned char data){
	while(UCB1STAT & UCBUSY);//make sure the bus is not in use
	P5OUT &= ~enpin; //set CS low to select chip
	UCB1TXBUF=addr;
	while(!(UC1IFG & UCB1TXIFG));//wait for room in tx bufer
	UCB1TXBUF=data;
	while(UCB1STAT & UCBUSY);//wait for communication to complete
	P5OUT |= enpin; //set CS high to deselect chip
	return;
}
/*void spitxrx(int enpin, unsigned char addr, unsigned char *data, unsigned int len, unsigned char inc){
	unsigned int i=0;
	unsigned char workingaddr=addr;
	while(!(UC1IFG & UCB1TXIFG));//make sure the bus is not in use
	P5OUT &= ~enpin; //set CS low to select chip
	spitx(enpin,addr);
	while(len-i){
		workingaddr += inc;
		spirx(enpin,workingaddr,&data[i]);
		i++;
	}
	//while(!(UC1IFG & UCB1RXIFG));//wait for last byte to be recieved
	//P5OUT |= enpin; //set CS high to deselect chip
	return;
}*/
