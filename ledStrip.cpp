/*
 * ledStrip.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: Aztec
 */
#include <dspi.h>
#include <pins.h>
#include <pinconstant.h>
#include <stdio.h>

#include "ledStrip.h"

LedStrip* LedStrip::currentStripSPI;

/***********************************************
 * @brief Initialize pins needed for DSPI      *
 ***********************************************/
LedStrip::LedStrip() {
//	iprintf("constructor\r\n");
	J2[25].function( PINJ2_25_DSPI1_SCK );
	J2[27].function( PINJ2_27_DSPI1_SIN );
	J2[28].function( PINJ2_28_DSPI1_SOUT );

//	iprintf("pins set \r\n");
	DSPIInit( DEFAULT_DSPI_MODULE, 2000000, 0x8, 0x0F, 0x0F, 0, 0, TRUE, 0, 0 );
}

/***********************************************
 * @brief LedStrip destructor                  *
 ***********************************************/
LedStrip::~LedStrip() {
	delete currentStripSPI;
}

/***********************************************
 * @brief Writes provided bytes to DSPI.       *
 *                                             *
 * @param bytePtr  - pointer to data           *
 *                   to write to SPI           *
 * @param numBytes - num bytes to write        *
 ***********************************************/
void LedStrip::WriteToDSPI( PBYTE bytePtr, int numBytes ) {
	DSPIStart( DEFAULT_DSPI_MODULE, bytePtr, NULL, numBytes, NULL);
	while( !DSPIdone( DEFAULT_DSPI_MODULE )) {}
}

/***********************************************
 * @brief Returns pointer to LedStripSPI.      *
 *                                             *
 * @return - pointer to currentStripSPI        *
 ***********************************************/
LedStrip* LedStrip::GetLedStrip() {
	if( currentStripSPI == NULL ) {
//		iprintf("current strip is null\r\n");
		currentStripSPI = new LedStrip();
	}
//	iprintf("got current strip spi. \r\n");
	return currentStripSPI;
}

/***********************************************
 * @brief Initializes the led strip (sends     *
 *        two clear bits via SPI).             *
 *                                             *
 * @return - TRUE if pass, FALSE if fail       *
 ***********************************************/
BOOL LedStrip::initLedStrip() {
	if( currentStripSPI == NULL ) {
		return FALSE;
	}

	currentStripSPI->WriteToDSPI( clearBytes , 2);

	return TRUE;
}

/***********************************************
 * @brief Sets color value of a single LED.    *
 *                                             *
 * @param i - index of LED                     *
 * @param r - red value                        *
 * @param g - green value                      *
 * @param b - blue value                       *
 ***********************************************/
void LedStrip::setLedValue(int i, uint8_t r, uint8_t g, uint8_t b) {
	ledStrip[i].setColorValue(r,g,b);
}


/***************************************************
 * @brief Sets the color value of the whole strip. *
 *                                                 *
 * @param r - red value                            *
 * @param g - green value                          *
 * @param b - blue value                           *
 ***************************************************/
void LedStrip::setStripColor(uint8_t r, uint8_t g, uint8_t b) {
	for( int i = 0; i < ledCount; i++ ) {
		ledStrip[i].setColorValue( r, g, b );
	}
}

/*********************************************
 * @brief Sets LEDs on the strip to white.   *
 *********************************************/
void LedStrip::setStripWhite() {
	for( int i = 0; i < ledCount; i++ ) {
		ledStrip[i].setColorlessValue();
	}
}

/**************************************************
 * @brief Sets the brightness value of the strip. *
 *        Call after setting colors and before    *
 *        writeLedStrip().                        *
 *                                                *
 * @param brightPercent - % brightness (0-100)    *
 **************************************************/
void LedStrip::modifyStripBrightness(uint8_t brightPercent) {
	for( int i = 0; i < ledCount; i++ ) {
		ledStrip[i].modifyBrightness(brightPercent);
	}
}

/*********************************************
 * Writes current LED values to DSPI and     *
 * resets the strip by passing 2 empty bits  *
 *********************************************/
void LedStrip::writeLedStrip() {
	for( int i = 0; i < ledCount; i++ ) {
		ledStrip[i].writeLedValues();
		iprintf("G:%d ",ledStrip[i].getColorValues(0));
		iprintf("R:%d ",ledStrip[i].getColorValues(1));
		iprintf("B:%d ",ledStrip[i].getColorValues(2));
		iprintf("I:%d\r",i);
	}
	currentStripSPI->WriteToDSPI( clearBytes, 2 );
}

/*********************************************
 * Turns off all LEDs on the strip.          *
 *********************************************/
void LedStrip::turnStripOff() {
	for( int i = 0; i < ledCount; i++ ) {
		//Turn strip off (fill with 128 value)
		ledStrip[i].setLedOff();
	}
	writeLedStrip();
}


