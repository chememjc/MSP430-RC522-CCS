#include <msp430.h>
#include <msp430f2419.h>
#include <stdio.h>
#include <string.h>
#include "rs232shell.h"
#include "globals.h"
#include "i2c.h"
#include "spi.h"
#include "rc522.h"
#include "rfid.h"

/*
 * main.c
 */

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    //Configure Oscillator
    DCOCTL = 0;
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;

    //put serial devices in reset
    UCA0CTL1 = UCSWRST;
    UCA1CTL1 = UCSWRST; //leave this one in reset since it is unused
    UCB0CTL1 = UCSWRST;
    UCB1CTL1 = UCSWRST;

    //Pin Assignments:
    //Port	Pin	Num	Assign	MyName	Comments
    //P1.0	12	12	GPO		RFIDRST	Reset control for the RFID chip (active low)
    //P1.1	13	13	GPI
    //P1.2	14	14	GPI
    //P1.3	15	15	GPI
    //P1.4	16	16	GPI
    //P1.5	17	26	GPI
    //P1.6	18	27	GPI
    //P1.7	19	28	GPI
    //P3.1  29	38	UCB0SDA		I2C
    //P3.2  30	39	UCB0SCL
    //P3.4	32	41	UCA0TXD		RS232 (debug and diagnostics)
    //P3.5	33	60	UCA0RXD
    //P3.6	34	61	UCA1TXD		RS232 (Spare)
    //P3.7	35	62	UCA1RXD
    //P5.1	45	72	UCB1SIMO	SPI
    //P5.2	46	73	UCB1SOMI
    //P5.3	47	74	UCB1CLK
    //P5.4	48	75	GPO		CSRFID	//chip select for RFID

    //Configure Pins
    // default to input with pulldown enabled
    //	  PxDIR	    PxREN	  PxOUT	    PxSEL
    //	 I=0 O=1    on/off	 pu=1 pd=0 0=GPIO 1=Other
    // x 01234567  01234567  01234567  0  1  2  3  4  5  6  7
    //P1 11110000  00001111  00000000  0  0  0  0  0  0  0  0
    //P2 00000000  11111111  00000000  0  0  0  0  0  0  0  0
    //P3 00001010  11110000  00000000  0  1  1  0  1  1  0  0
    //P4 00000000  11111111  00000000  0  0  0  0  0  0  0  0
    //P5 00001100  11110011  00000000  0  1  1  1  0  0  0  0
    //P6 00000000  11111111  00000000  0  0  0  0  0  0  0  0
    P1DIR = BIT0 + BIT1 + BIT2 + BIT3;
    P1OUT = 0;
    P2DIR = 0;
    P3DIR = BIT4 + BIT6;
    P4DIR = 0;
    P5DIR = BIT4 + BIT5; //bit 4 =chip select bit 5 =reset
    P5OUT = BIT4;		 //start with RC522 in reset
    P6DIR = 0;
    P7DIR = 0;
    P8DIR = 0;
    P1REN = 0x0F;
    P2REN = 0xFF;
    P3REN = 0xF0;
    P4REN = 0xFF;
    P5REN = 0xFF&(~(BIT4+BIT5));
    P6REN = 0xFF;
    P7REN = 0xFF;
    P8REN = 0xFF;
    P1SEL = 0;
    P2SEL = 0;
    P3SEL = BIT1 + BIT2 + BIT4 + BIT5 + BIT6 + BIT7;
    P4SEL = 0;
    P5SEL = BIT1 + BIT2 + BIT3;
    P6SEL = 0;
    P7SEL = 0;
    P8SEL = 0;

    //Configure UCA and UCB
    //	UCA0 = RS232 9600-8-n-1
    //	UCA1 = RS232 9600-8-n-1
    //	UCB0 = I2C
    //	UCB1 = SPI
    UCA0CTL1 = UCSWRST;
    UCA1CTL1 = UCSWRST; //Alternate UART configuration
    UCB0CTL1 = UCSWRST;
    UCB1CTL1 = UCSWRST;

    /*//Configure UCA0 for RS232:
    UCA0CTL1 |= UCSSEL_2;	// SMCLK/16  UCSSEL_2 for 1mhz
    UCA0BR0 = 104;			// 1MHz 9600 = 104
    UCA0BR1 = 0; 			// 1MHz 9600 = 0
    UCA0MCTL =UCBRS0;		// Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;	//Enable UCA0*/
    UCA0CTL1 |= UCSSEL_2;	// SMCLK/16  UCSSEL_2 for 1mhz
    UCA0BR0 = 1666 & 0x00FF;			// 1MHz 9600 = 104
    UCA0BR1 = (1666 & 0xFF00)>>8; 			// 1MHz 9600 = 0
    UCA0MCTL =UCBRS0;		// Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;	//Enable UCA0

    //Configure UCA1 for RS232:
    UCA1CTL1 |= UCSSEL_2;	// SMCLK/16 UCCSEL_2 for 1mhz
    UCA1BR0 = 104;			// 1MHz 9600 = 104
    UCA1BR1 = 0; 			// 1MHz 9600 = 0
    UCA1MCTL = UCBRS0;		// Modulation UCBRSx = 1
    UCA1CTL1 &= ~UCSWRST;	//Enable UCA1

    //Enable interrupt for UCA0:
    UCA0CTL1 |= UCRXEIE;		// Enable Rx interrupt
    IE2 = UCA0RXIE;		// Enable global Rx interrupt

    //Configure UCB0 for I2C
    i2cinit(10); //baud rate selection; smclk/x; x=10 @1MHz clock and 100kHz I2c

    //Configure UCB1 for SPI
    spisetup(4, RC522EN); //set SPI for 4 MHz (max 10 MHz for RC522)

    //init the RC522
    initRC522(); //RC522

    __bis_SR_register(GIE); // interrupts enable
    rxbuffer[RXBUFFERLEN-1]=0; //make sure the command buffer is null-terminated
    //Display startup message:
    unsigned int i;
    serout("\r\n\r\nDoor Controller: Port A0\r\n");
    const char *str2 = "\r\n\r\nDoor Controller: Port A1\r\n";
    for(i=0;i<strlen(str2);i++){
       	while (!(IFG2&UCA1TXIFG)); // USCI_A0 TX buffer ready?
     	UCA1TXBUF = str2[i];
    }
    serout(prompt); //send serial prompt; we're ready
    unsigned long int loop=5000;
	while(1){
		//main program loop
		handleinput(); //
		if(loop==0) {
			loop=0x2FFFF;
			scanforcard();
		}
		else loop--;
	}
}

// *************** Interrupsercmd |= RXEVALt Routines ***************
/* Echo back RXed character, confirm TX buffer is ready first */
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)  //echo
{
    if(IFG2 & UCA0RXIFG){ //serial terminal ISR
    	if(UCA0RXBUF<=127){
    		if(UCA0RXBUF>=32){
    			rxbuffer[rxptr]=UCA0RXBUF;
    			rxptr++;
    			while (!(IFG2&UCA0TXIFG)); // USCI_A0 TX buffer ready?
    			UCA0TXBUF = UCA0RXBUF; // echo character
    			toggleLED(LED1);
    		}
    		else if (UCA0RXBUF==3){ //control-c
    			rxptr=0;
    			sercmd |= TXCTLC;
    		}
    		else if (UCA0RXBUF=='\r'){ //hit enter, execute command
    			rxbuffer[rxptr]=0;//null terminate string
    			rxptr=0; //put pointer back at beginning of string
    			//sercmd |= TXNL;
    			sercmd |= RXEVAL;
    		}
    		else if (UCA0RXBUF=='\b'){
    			if(rxptr != 0){
    				rxptr--;
    				sercmd |= TXBS;
    			}
    		}
    	}
    	else toggleLED(LED2); //invalid character sent
    	if(rxptr==RXBUFFERLEN-2){
    		sercmd |= EBUFF;
    		rxptr=0;
    	}
    //clear the flag
    //IFG2 &= ~UCA0RXIFG;
    } //end USCI A0 interrupt service

    //for UCSI B0 I2C interrupt service
    if (UCB0STAT & UCNACKIFG){            // send STOP if slave sends NACK
        UCB0CTL1 |= UCTXSTP;
        UCB0STAT &= ~UCNACKIFG;
    } //end USCI B0 interrupt service
    //__delay_cycles(1000000);
}
