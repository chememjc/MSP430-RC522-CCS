# RC522 library for Code Composer Studio 6.1
A ported version of the Energia RF522 library that works directly in CCS 6.1 (probably earlier versions too).  Original library was from https://github.com/choffee/Energia-Mfrc522/ .  The library in its current form is a straight C library with no C++ involved; this is strictly based upon my personal preference of C over C++ in embedded applications.  

#Supported Processors
The project is built for a MSP430F2419 but should work on any chip with a USCI with some trivial changes.  The SPI port is configured to use UCB1 but will also work on UCA0, UCA1, and UCB0 with some changes.  These changes would include the spi.c and spi.h files where all references to UCB1 should be changed to the appropriate port.  Additionally, the interrupt flags are in different registers for UCx0 and UCx1; UCx0 interrupt flags live in register IFG2 and UCx1 flags live in register UC1IFG.  Porting the routines over to UCA1 should be as simple as changing all of the UCB1 occurances to UCA1.  For UCA0 or UCB0 all occurances of UCB1 will need to be changed to either UCA0 or UCB0 *AND* all occurances of UC1IFG will need to be changed to IFG2 in the spi.c file.

To build this project on a chip without a USCI, only the functions in spi.c will need to be rewritten, everything else calls them for SPI communication.

#Other Components
UCAO/UCA1: A serial UART is used to help with debugging, it has a modifyable command set to trigger particular events.  Its not terribly elegant, but it does what it needs to do.  This is configured on UCA0.  UCB0 is also configured as a serial UART, though it goes unused in this project after being configured.

UCB0: I2C bus is configured

UCB1: SPI (for MFRC522 communication)
