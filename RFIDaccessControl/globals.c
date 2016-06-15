/*
 * globals.c
 *
 *  Created on: Jun 9, 2016
 *      Author: mcurtis
 */

#include "globals.h"

char rxbuffer[RXBUFFERLEN];
const char *prompt = "\r\nrs232@door> ";
volatile unsigned int rxptr=0, rxwptr=0, sercmd=0; //the pointer for the serial line buffer, working pointer for main code

