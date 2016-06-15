/*
 * serial.c
 *
 *  Created on: Jun 9, 2016
 *      Author: mcurtis
 */
#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include "rs232shell.h"
#include "globals.h"
#include "i2c.h"
#include "spi.h"
#include "pcf8574lcd.h"
#include "rc522.h"
#include "rfid.h"

void serout(const char *str){
	unsigned int i;
	for(i=0;i<strlen(str);i++){
		while (!(IFG2&UCA0TXIFG)); // USCI_A0 TX buffer ready?
		UCA0TXBUF = str[i];
	}
}
/*void serout(){
	//use DMA channel 0 to send data to UCA0
	//not available on msp430f2419
}*/

void handleinput(){
	if((sercmd&TXCTLC)){
		sercmd = 0;
		serout("^C");
		serout(prompt);
	}
	if((sercmd&TXNL)){
		sercmd &= ~TXNL;
		while (!(IFG2&UCA1TXIFG));
		UCA0TXBUF = '\r';
		while (!(IFG2&UCA1TXIFG));
		UCA0TXBUF = '\n';
	}
	if((sercmd&EBUFF)) {
		sercmd &= ~EBUFF; //clear flag
		strcpy(rxbuffer,"E\r\nBuffer length exceeded");
		sercmd |= RXEVAL;
	}
	if((sercmd&TXBS)) {
		sercmd &= ~TXBS; //clear flag
		serout("\b \b");
	}
	if((sercmd&RXEVAL)){
		sercmd &= ~RXEVAL; //Handle event (ignore all others)
		//evaluate command sent via serial
		rxwptr=1;
		if(rxbuffer[0]=='E'){
			//echo text after
			while(rxbuffer[rxwptr] && rxwptr<RXBUFFERLEN){
			while (!(IFG2&UCA0TXIFG));
				UCA0TXBUF = rxbuffer[rxwptr];
				rxwptr++;
			}
		}
		if(rxbuffer[0]=='L'){
			if(rxbuffer[1]=='0') toggleLED(LED0);
			else if (rxbuffer[1]=='1') toggleLED(LED1);
			else if (rxbuffer[1]=='2') toggleLED(LED2);
			else if (rxbuffer[1]=='3') toggleLED(LED3);
		}
		if(rxbuffer[0]=='h' || rxbuffer[0]=='?'){
			serout("\r\nh or ? -This help\r\nE{str} -echo {str}\r\nLx -Toggle LEDx where x=1-4");
		}
		if(rxbuffer[0]=='i' && rxbuffer[1]=='l'){ //ils list stuff on the i2c bus
			//list whats on the i2c bus
			unsigned char i;
			char str[15];
			serout("\r\nHex  Dec");
			for(i=127; i!=0; i--){
				if(i2cslavecheck(i)) {
					sprintf(str,"\r\n0x%x %d",i,i);
					serout(str);
				}
			}
		}
		if(rxbuffer[0]=='c' && rxbuffer[1]=='l'){
			//clear LCD
			initLCD(LCDADDR);
			serout("\r\nDisplay Cleared");
		}
		if(rxbuffer[0]=='i' && rxbuffer[1]=='w'){
			//try i2c write
			unsigned char data[3]={'a','b','c'};
			TI_USCI_I2C_transmitinit(LCDADDR,I2CPRESCALER);  // init transmitting with
			while ( TI_USCI_I2C_notready() );         // wait for bus to be free
			TI_USCI_I2C_transmit(3,data);       // start transmitting
			serout("\r\nTx test done");
		}
		if(rxbuffer[0]=='r'){
			if(rxbuffer[2]=='i'){ //rcinit
				P3OUT &= ~RC522RST;
				P3OUT |= RC522EN;
				__delay_cycles(1000); //wait a millisecond before bringing the chip back up
				initRC522();
				serout("\r\nRC522 init complete");
			}
			else if(rxbuffer[2]=='p'){ //rcpoll
				//poll the reader to see if we have something conected
				scanforcard();
			}
			else if(rxbuffer[2]=='h'){ //rfhalt
				haltRC522();
				serout("RC522 card halted");
			}
		}
		if(rxbuffer[0]=='s'){
			unsigned char testval;
			char test[11];
			//spitx(RC522EN,(0x2A<<1)&0x7E);
			//spirx(RC522EN,(0x2A<<1)&0x7E|0x80,&testval);
			//sprintf(test, "\r\nrx:0x%x", testval);
			//serout(test);
			spitxrx(RC522EN,((0x11<<1)&0x7E)|0x80, &testval, 1, 0);
			sprintf(test, "\r\nrx:0x%x", testval);
			serout(test);
			spitxx(RC522EN,(0x11<<1)&0x7E, 0x3D);
			__delay_cycles(100000);
			testme();
		}
		serout(prompt);
	}
	return;
}
