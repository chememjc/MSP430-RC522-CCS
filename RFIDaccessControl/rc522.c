/*
 * rc522.c
 *
 *  Created on: Jun 11, 2016
 *      Author: mcurtis
 */
#include <msp430.h>
#include <stdlib.h>
#include "spi.h"
#include "rc522.h"
#include "globals.h"
#include "rs232shell.h"
#include <stdio.h>

void initRC522(){
	//this routine expects the SPI bus to already be configured
	//the MFRC522 chip supports up to 10 Mbit communiation, so set the USCI accordingly
	//bring reset high and wait for chip to wake up:
	P5OUT &= ~RC522RST;
	__delay_cycles(50000);  //wait 50 milliseconds for full-on shutdown
	P5OUT |= RC522RST;
	__delay_cycles(50000);  //wait 50 milliseconds for startup
	/*writeRC522(TModeReg, 0x8D);	//Tauto=1, f(timer)=6.78 MHz/TPreScaler was 8D
	writeRC522(TPrescalerReg, 0x3E); //was 3E
	writeRC522(TReloadRegL, 1); //was dec30
	writeRC522(TReloadRegH, 30);  //was 0
	writeRC522(TxAutoReg, 0x40);
	writeRC522(ModeReg, 0x3D);
	writeRC522(RFCfgReg, regvar[RFCfgReg][0]); //max power
	writeRC522(CommandReg, PCD_IDLE);
	writeRC522(FIFOLevelReg, BIT7); //flush FIFO*/
	writeRC522(TModeReg, regvar[TModeReg][0]);	//Tauto=1, f(timer)=6.78 MHz/TPreScaler was 8D
	writeRC522(TPrescalerReg, regvar[TPrescalerReg][0]); //was 3E
	writeRC522(TReloadRegL, regvar[TReloadRegL][0]); //was dec30
	writeRC522(TReloadRegH, regvar[TReloadRegH][0]);  //was 0
	writeRC522(TxAutoReg, regvar[TxAutoReg][0]);
	writeRC522(ModeReg, regvar[ModeReg][0]);
	writeRC522(RFCfgReg, regvar[RFCfgReg][0]); //max power
	writeRC522(CommandReg, PCD_IDLE);
	writeRC522(FIFOLevelReg, BIT7); //flush FIFO
	/*//now verify:
	if(!verifyRC522(TModeReg, regvar[TModeReg][0],regvar[TModeReg][1])) serout("\r\nVfail:TModeReg");
	if(!verifyRC522(TPrescalerReg, regvar[TPrescalerReg][0],regvar[TPrescalerReg][1])) serout("\r\nVfail:TPrescalerReg");
	if(!verifyRC522(TReloadRegL, regvar[TReloadRegL][0],regvar[TReloadRegL][1])) serout("\r\nVfail:TReloadRegL");
	if(!verifyRC522(TReloadRegH, regvar[TReloadRegH][0],regvar[TReloadRegH][1])) serout("\r\nVfail:TReloadRegH");
	if(!verifyRC522(TxAutoReg, regvar[TxAutoReg][0],regvar[TxAutoReg][1])) serout("\r\nVfail:TxAutoReg");
	if(!verifyRC522(ModeReg, regvar[ModeReg][0],regvar[ModeReg][1])) serout("\r\nVfail:ModeReg");
	if(!verifyRC522(RFCfgReg, regvar[RFCfgReg][0],regvar[RFCfgReg][1])) serout("\r\nVfail:RFCfgReg");*/
	//turn antenna on:
	antennaRC522(1);
	//setbitRC522(TxControlReg, 0x03); //does same thing as antennaRC522(1)
	return;
}

inline void writeRC522(unsigned char addr, unsigned char value){
	//set a particular register to a value
	spitxx(RC522EN, (addr<<1)&RC522WRITE, value);
	return;
}

inline void readmRC522(unsigned char addr, unsigned char *value, unsigned char n, unsigned char inc){
	//read multiple from the same address (ex:FIFO)
	spitxrx(RC522EN, ((addr<<1)&RC522WRITE) | RC522READ, value, n,inc);
	return;
}

inline void readRC522(unsigned char addr, unsigned char *value){
	//read a register's value
	spitxrx(RC522EN, ((addr<<1)&RC522WRITE) | RC522READ, value, 1,0);
	return;
}

void testme(){
	char test[11];
	unsigned char testval;
	writeRC522(ModeReg, 0x3D);
	readRC522(ModeReg, &testval);
	sprintf(test, "\r\nrx:0x%x", testval);
	serout(test);
	return;
}

void clearbitRC522(unsigned char addr, unsigned char mask){
	//clear a bit
	unsigned char tmp;
	readRC522(addr, &tmp);
	writeRC522(addr, tmp & (~mask));
	return;
}

void setbitRC522(unsigned char addr, unsigned char mask){
	//set a bit
	unsigned char tmp;
	readRC522(addr, &tmp);
	writeRC522(addr, tmp | mask);
	return;
}

void antennaRC522(unsigned char state){
	//state==0 turn it off
	//state==1 turn it on
	if(state) setbitRC522(TxControlReg, 0x03);	//turn antenna on
	else clearbitRC522(TxControlReg, 0x03);		//turn antenna off
	return;
}

unsigned char verifyRC522(unsigned char addr, unsigned char data, unsigned char mask){
	//return 0 if register matches, return 1 if it does not
	unsigned char regval;
	readRC522(addr, &regval);
	if((data&mask)==(regval&mask)) return 1; //data matches
	char stringy[32];
	sprintf(stringy, "\r\nReg:0x%x: 0x%x, set, now 0x%x",addr,regval,regvar[TReloadRegH][0]);
	serout(stringy);
	return 0;
}

inline void FIFOclearRC522(){
	setbitRC522(FIFOLevelReg,B8(10000000));
	return;
}
void FIFOwriteRC522(unsigned char *data, unsigned char length){
	//write contents to FIFO
	unsigned int i=0;
	while(i<length){
		writeRC522(FIFODataReg, data[i]);
		i++;
	}
	return;
}

int FIFOreadRC522(unsigned char *data, unsigned char length){
	//read the contents of the FIFO
	unsigned char i=0,n,retval;
	readRC522(FIFOLevelReg, &n);
	n &= B8(00111111); //pull out the length of the buffer
	retval=n;
	if(n>length) n=length;	//don't overflow the array that was passed in!
	while(i<n){
		readRC522(FIFODataReg, &data[n-i]);
		i++;
	}
	return retval;  //current buffer length (max 'length' bytes returned even if buffer length is larger
}

inline void resetRC522(){
	writeRC522(CommandReg, PCD_RESETPHASE);
	return;
}

unsigned char requestRC522(unsigned char reqMode, unsigned char *TagType){
	//search card, read card type
	unsigned char status;
	unsigned int backBits=MAX_LEN;

	writeRC522(BitFramingReg, 0x07);
	TagType[0] = reqMode;
	status = tocardRC522(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
	//if ((status != MI_OK) || (backBits != 0x10)) status = MI_ERR;
	return status;
}

/*unsigned char tocardRC522(unsigned char command, unsigned char *sendData,
		unsigned char sendLen, unsigned char *backData, unsigned int *backLen)
{
	unsigned char status = MI_ERR;
	unsigned char irqEn = 0x00;
	unsigned char waitIRq = 0x00;
	unsigned char lastBits;
	unsigned char ereg;
	unsigned char n;
	int i;

	switch (command)
	{	//CommIEnReg Bit7=Pin Polarity, Bit6=TxIEn, Bit5=RxIEn, Bit4=IdleEn, Bit3=HiAlert, Bit2=LoAlert
		//			 Bit1=ErrIEn, Bit0=TimerIEn
		case PCD_AUTHENT:
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE:
		{
			//irqEn = 0x77;
			irqEn = 0x77; //rx IRQ and IdleIRQ
			waitIRq = 0x30;
			break;
		}
		default:
			break;
	}

	writeRC522(CommandReg, PCD_IDLE); //stop any active command
	writeRC522(CommIEnReg, irqEn);	//enable appropriate IRQs
	writeRC522(CommIrqReg, 0x7F); //clear all interrupt flags
	setbitRC522(FIFOLevelReg, 0x80); //

	for (i=0; i<sendLen; i++)
	{
		writeRC522(FIFODataReg, sendData[i]);
	}

	writeRC522(CommandReg, command);
	if (command == PCD_TRANSCEIVE)
	{
		writeRC522(BitFramingReg, 0x80);		//StartSend=1,transmission of data starts
	}

	i = 0xFFFF;
	while(1){
		readRC522(CommIrqReg, &n);
		if(n & waitIRq){
			//one of the interupts signaling success has been sent
			serout("\r\n waitIRQ success");
			break;
		}
		if(n&BIT0){
			//on-chip timeout has occured, nothing responded
			//serout("\r\n chip timeout fail");
			return MI_ERR;
		}
		if(--i==0){
			// we've waited too long on the host processor
			//serout("\r\n host timeout fail");
			return MI_ERR;
		}
	}

	//clearbitRC522(BitFramingReg, 0x80);			//StartSend=0


	readRC522(ErrorReg, &ereg);
	if((ereg & 0x13)){//BufferOvfl ParityErr ProtocolErr
		return MI_ERR;  //
	}
	if(backLen) //as long as a return value is desired, send it!
	{
		status = MI_OK;
		if (n & irqEn & 0x01)
		{
			status = MI_NOTAGERR;			//??
		}

		if (command == PCD_TRANSCEIVE)
		{
			readRC522(FIFOLevelReg, &n);
			*backLen=n;
			readRC522(ControlReg,&lastBits);
			lastBits &= 0x07; //pull out bits 2:0 with # of valid return bits



			if (n > MAX_LEN) n = MAX_LEN;
			for (i=0; i<n; i++)
			{
				readRC522(FIFODataReg, &backData[i]);
			}
		}
	}
	else
	{
		status = MI_ERR;
	}

//setbitRC522(ControlReg,0x80);           //timer stops //was commented out
//writeRC522(CommandReg, PCD_IDLE);						//was commented out

	return status;
}*/
unsigned char tocardRC522(unsigned char command, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned int *backLen)
{
	unsigned char status = MI_ERR;
	unsigned char ereg;
	unsigned char irqEn = 0x00;
	unsigned char waitIRq = 0x00;
	unsigned char lastBits;
	unsigned char n;
	unsigned int i;

	switch (command)
	{
		case PCD_AUTHENT:
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE:
		{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
			break;
	}

	writeRC522(CommIEnReg, irqEn|0x80);
	clearbitRC522(CommIrqReg, 0x80);
	setbitRC522(FIFOLevelReg, 0x80);

	writeRC522(CommandReg, PCD_IDLE);

	for (i=0; i<sendLen; i++)
	{
		writeRC522(FIFODataReg, sendData[i]);
	}

	writeRC522(CommandReg, command);
	if (command == PCD_TRANSCEIVE)
	{
		setbitRC522(BitFramingReg, 0x80);		//StartSend=1,transmission of data starts
	}

	i = 2000;
	do
	{
		readRC522(CommIrqReg, &n);
		i--;
	}
	while ((i!=0) && !(n&0x01) && !(n&waitIRq));

	clearbitRC522(BitFramingReg, 0x80);			//StartSend=0

	if (i != 0)
	{
		readRC522(ErrorReg, &ereg);
		//if(!(ReadReg(ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
		if(!(ereg & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
		{
			status = MI_OK;
			if (n & irqEn & 0x01)
			{
				status = MI_NOTAGERR;			//??
			}

			if (command == PCD_TRANSCEIVE)
			{
				readRC522(FIFOLevelReg, &n);
				readRC522(ControlReg, &lastBits);
				lastBits &= 0x07;
				if (lastBits)
				{
					*backLen = (n-1)*8 + lastBits;
				}
				else
				{
					*backLen = n*8;
				}
				if (n == 0)
				{
					n = 1;
				}
				if (n > MAX_LEN)
				{
					n = MAX_LEN;
				}

				readmRC522(FIFODataReg, backData,n,0);
			}
		}
		else
		{
			status = MI_ERR;
		}
	}

//setbitRC522(ControlReg,0x80);           //timer stops
//writeRC522(CommandReg, PCD_IDLE);

	return status;
}


/*
 * Function：MFRC522_Anticoll
 * Description：Prevent conflict, read the card serial number
 * Input parameter：serNum--return the 4 bytes card serial number, the 5th byte is recheck byte
 * return：return MI_OK if successed
 */
unsigned char anticollRC522(unsigned char *serNum)
{
	unsigned char status;
	unsigned char i;
	unsigned char serNumCheck=0;
	unsigned int unLen;


//clearbitRC522(Status2Reg, 0x08);		//TempSensclear
//clearbitRC522(CollReg,0x80);			//ValuesAfterColl
	writeRC522(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]
	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = tocardRC522(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

	if (status == MI_OK)
	{
		for (i=0; i<4; i++)
		{
			serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i])
		{
			//wrong checksum
			status = MI_ERR;
		}
	}

	//setbitRC522(CollReg, 0x80);		//ValuesAfterColl=1

	return status;
}


/*
 * Function：CRCcalcRC522
 * Description：Use MF522 to calculate CRC
 * Input parameter：pIndata--the CRC data need to be read，len--data length，pOutData-- the caculated result of CRC
 * return：Null
 */
void CRCcalcRC522(unsigned char *pIndata, unsigned char len, unsigned char *pOutData)
{
	unsigned char i, n;

	clearbitRC522(DivIrqReg, 0x04);			//CRCIrq = 0
	setbitRC522(FIFOLevelReg, 0x80);
	//writeRC522(CommandReg, PCD_IDLE);

	for (i=0; i<len; i++)
	{
		writeRC522(FIFODataReg, *(pIndata+i));
	}
	writeRC522(CommandReg, PCD_CALCCRC);

	i = 0xFF;
	do
	{
		readRC522(DivIrqReg, &n);
		i--;
	}
	while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	readRC522(CRCResultRegL,&pOutData[0]);
	readRC522(CRCResultRegM,&pOutData[1]);
}


/*
 * Function：SelectTag
 * Description：Select card, read card storage volume
 * Input parameter：serNum--Send card serial number
 * return：return the card storage volume
 */
unsigned char selectRC522(unsigned char *serNum)
{
	unsigned char i;
	unsigned char status;
	unsigned char size;
	unsigned int recvBits;
	unsigned char buffer[9];

	//clearbitRC522(Status2Reg, 0x08);			//MFCrypto1On=0

	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	for (i=0; i<5; i++)
	{
		buffer[i+2] = *(serNum+i);
	}
	CRCcalcRC522(buffer, 7, &buffer[7]);		//??
	status = tocardRC522(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

	if ((status == MI_OK) && (recvBits == 0x18))
	{
		size = buffer[0];
	}
	else
	{
		size = 0;
	}

	return size;
}


/*
 * Function：Auth
 * Description：verify card password
 * Input parameters：authMode--password verify mode
                 0x60 = verify A password key
                 0x61 = verify B password key
             BlockAddr--Block address
             Sectorkey--Block password
             serNum--Card serial number ，4 bytes
 * return：return MI_OK if successed
 */
unsigned char authRC522(unsigned char authMode, unsigned char BlockAddr, unsigned char *Sectorkey, unsigned char *serNum)
{
	unsigned char status;
	unsigned int recvBits;
	unsigned char i,temp;
	unsigned char buff[12];

	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i=0; i<6; i++)
	{
		buff[i+2] = *(Sectorkey+i);
	}
	for (i=0; i<4; i++)
	{
		buff[i+8] = *(serNum+i);
	}
	status = tocardRC522(PCD_AUTHENT, buff, 12, buff, &recvBits);

	readRC522(Status2Reg,&temp);
	if ((status != MI_OK) || (!(temp & 0x08)))
	{
		status = MI_ERR;
	}

	return status;
}


/*
 * Function：ReadBlock
 * Description：Read data
 * Input parameters：blockAddr--block address;recvData--the block data which are read
 * return：return MI_OK if successed
 */
unsigned char readblockRC522(unsigned char blockAddr, unsigned char *recvData)
{
	unsigned char status;
	unsigned int unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	CRCcalcRC522(recvData,2, &recvData[2]);
	status = tocardRC522(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

	if ((status != MI_OK) || (unLen != 0x90))
	{
		status = MI_ERR;
	}

	return status;
}


/*
 * Function：WriteBlock
 * Description：write block data
 * Input parameters：blockAddr--block address;writeData--Write 16 bytes data into block
 * return：return MI_OK if successed
 */
unsigned char writeblockRC522(unsigned char blockAddr, unsigned char *writeData)
{
	unsigned char status;
	unsigned int recvBits;
	unsigned char i;
	unsigned char buff[18];

	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	CRCcalcRC522(buff, 2, &buff[2]);
	status = tocardRC522(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
	{
		status = MI_ERR;
	}

	if (status == MI_OK)
	{
		for (i=0; i<16; i++)
		{
			buff[i] = *(writeData+i);
		}
		CRCcalcRC522(buff, 16, &buff[16]);
		status = tocardRC522(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
		{
			status = MI_ERR;
		}
	}

	return status;
}


/*
 * Function：Halt
 * Description：Command the cards into sleep mode
 * Input parameters：null
 * return：null
 */
unsigned char haltRC522(void)
{
	unsigned char status;
	unsigned char buff[4];

	buff[0] = PICC_HALT;
	buff[1] = 0;
	CRCcalcRC522(buff, 2, &buff[2]);

	status = tocardRC522(PCD_TRANSCEIVE, buff, 4, NULL, 0);
	return status;
}


const unsigned char regvar[64][2]={
		//Initial Val,	Read Bitmask  where read bitmask==bits that can be modified and read-back
		//Reserved00,0x00
		{NULL,		0},					//#0x00
		//CommandReg,0x01
		{B8(00100000),	B8(00111111)},	//#0x01
		//CommIEnReg,0x02
		{B8(01100001),	B8(11111111)},	//#0x02
		//DivlEnReg,0x03
		{B8(10010100),	B8(10010100)},	//#0x03
		//CommIrqReg,0x04
		{B8(00000000),	B8(11111111)},	//#0x04
		//DivIrqReg,0x05
		{B8(00000000),	B8(00010100)},	//#0x05
		//ErrorReg,0x06
		{NULL,		0},					//#0x06
		//Status1Reg,0x07
		{NULL,		0},					//#0x07
		//Status2Reg,0x08
		{B8(00000000),	B8(11001000)},	//#0x08
		//FIFODataReg,0x09
		{NULL,		0},					//#0x09
		//FIFOLevelReg,0x0A
		{B8(10000000),	0},				//#0x0A
		//WaterLevelReg,0x0B
		{B8(00000000),	B8(00111111)},	//#0x0B
		//ControlReg,0x0C
		{B8(00000000),	B8(00000000)},	//#0x0C
		//BitFramingReg,0x0D
		{B8(00000000),	B8(01110111)},	//#0x0D
		//CollReg,0x0E
		{B8(10000000),	B8(10000000)},	//#0x0E
		//Reserved01,0x0F
		{NULL,		0},					//#0x0F
		//Reserved10,0x10
		{NULL,		0},					//#0x10
		//ModeReg,0x11
		{0x3D		,	B8(10101011)}, 	//#0x11*0x3D
		//TxModeReg,0x12
		{B8(00000000),	B8(11111000)},	//#0x12
		//RxModeReg,0x13
		{B8(00000100),	B8(11111100)},	//#0x13
		//TxControlReg,0x14
		{B8(00001011),	B8(11111011)},	//#0x14
		//TxAutoReg,0x15
		{0x40		,	B8(01000000)},	//#0x15*0x40//TxASKreg
		//TxSelReg,0x16
		{B8(00100100),	B8(00111111)},	//#0x16//MFOUT function?
		//RxSelReg,0x17
		{B8(10000110),	B8(11111111)},	//#0x17//0x86//delay time?
		//RxThresholdReg,0x18
		{B8(00100010),	B8(11110111)},	//#0x18//Rx minP, collP?
		//DemodReg,0x19
		{B8(00001010),	B8(11111111)},	//#0x19//danger?
		//Reserved11,0x1A
		{NULL,		0},					//#0x1A
		//Reserved12,0x1B
		{NULL,		0},					//#0x1B
		//MifareReg,0x1C
		{B8(00000011),	B8(00000011)},	//#0x1C
		//Reserved13,0x1D
		{NULL,		0},					//#0x1D
		//Reserved14,0x1E
		{NULL,		0},					//#0x1E
		//SerialSpeedReg,0x1F
		{0xEB,		B8(11111111)},		//#0x1F//datasheet p12
		//Reserved20,0x20
		{NULL,		0},					//#0x20
		//CRCResultRegM,0x21
		{B8(00000000),	0}, 			//#0x21//msb
		//CRCResultRegL,0x22
		{B8(00000000),	0}, 			//#0x22//lsb
		//Reserved21,0x23
		{NULL,		0},					//#0x23
		//ModWidthReg,0x24
		{B8(01000000),	B8(11111111)},	//#0x24//modulation wd?
		//Reserved22,0x25
		{NULL,		0},					//#0x25
		//RFCfgReg,0x26
		{0x7F		,	B8(01110000)},	//#0x26//*0x07<<4//power(maxed)
		//GsNReg,0x27
		{B8(00000000),	B8(00000000)},	//#0x27//don't touch it?
		//CWGsPReg,0x28
		{B8(00000000),	B8(00000000)},	//#0x28//don't touch it?
		//ModGsPReg,0x29
		{B8(00000000),	B8(00000000)},	//#0x29//don't touch it?
		//TModeReg,0x2A
		{0x8D		,	B8(11111111)},	//#0x2A//*0x80//timer prescale-set
		//TPrescalerReg,0x2B
		{0x3E		,	B8(11111111)},  //#0x2B//*0xA9//timer prescale-set
		//TReloadRegH,0x2C
		{0x00		,	B8(11111111)},	//#0x2C//*0x03//timer preloadH
		//TReloadRegL,0x2D
		{0x30		,	B8(11111111)},	//#0x2D//*0xE8//timer preloadL
		//TCounterValueRegH,0x2E
		{NULL		,	0},				//#0x2E
		//TCounterValueRegL,0x2F
		{NULL		,	0},				//#0x2F
		//Reserved30,0x30
		{NULL,			0},				//#0x30
		//TestSel1Reg,0x31
		{B8(00000000),	0},				//#0x31
		//TestSel2Reg,0x32
		{B8(00000000),	0},				//#0x32
		//TestPinEnReg,0x33
		{B8(00000000),	0},				//#0x33
		//TestPinValueReg,0x34
		{B8(00000000),	0},				//#0x34
		//TestBusReg,0x35
		{B8(00000000),	0},				//#0x35
		//AutoTestReg,0x36
		{B8(00000000),	0},				//#0x36
		//VersionReg,0x37
		{B8(00000000),	0},				//#0x37
		//AnalogTestReg,0x38
		{B8(00000000),	0},				//#0x38
		//TestDAC1Reg,0x39
		{B8(00000000),	0},				//#0x39
		//TestDAC2Reg,0x3A
		{B8(00000000),	0},				//#0x3A
		//TestADCReg,0x3B
		{B8(00000000),	0},				//#0x3B
		//Reserved31,0x3C
		{NULL,			0},				//#0x3C
		//Reserved32,0x3D
		{NULL,			0},				//#0x3D
		//Reserved33,0x3E
		{NULL,			0},				//#0x3E
		//Reserved34,0x3F
		{NULL,			0}};			//#0x3F
