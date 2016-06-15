/*
 * rfid.c
 *
 *  Created on: Jun 12, 2016
 *      Author: mcurtis
 */
#include <msp430.h>
#include <stdio.h>
#include <stddef.h>
#include "globals.h"
#include "rc522.h"
#include "rs232shell.h"

void loadconfig(){
	//load up the RC522 with default values

	return;
}
int scanforcard(){
	//scan for a card
    unsigned char rcid[MAX_LEN], rcstatus;
    char rcs[32]; //the text string for the serial number
	rcstatus = requestRC522(PICC_REQALL, rcid);
	if(!rcstatus) {
		P1OUT |= LED3;
		if(rcid[1]==0x00 && rcid[1]==0x44)      serout("\r\nMifare_UltraLight: ");
		else if(rcid[1]==0x00 && rcid[0]==0x04) serout("\r\nMifare_One: ");
		else if(rcid[1]==0x00 && rcid[0]==0x02) serout("\r\nMifare_One(S70): ");
		else if(rcid[1]==0x00 && rcid[0]==0x08) serout("\r\nMifare_Pro(X): ");
		else if(rcid[1]==0x03 && rcid[0]==0x44) serout("\r\nMifare_DESFire: ");
		else {
			sprintf(rcs, "\r\n%x:%x: ", rcid[0], rcid[1]);
			serout(rcs);
		}
	}
	else P1OUT &= ~LED3;
	rcstatus = anticollRC522(rcid);
	if(rcstatus == MI_OK){
		P1OUT |= LED2;
		sprintf(rcs,"0x%x-0x%x-0x%x-0x%x-0x%x",rcid[0],rcid[1],rcid[2],rcid[3],rcid[4]);
		serout(rcs);
		selectRC522(rcid);
		initRC522();
	}
	else P1OUT &= ~LED2;
	haltRC522();
	return rcstatus;
}
