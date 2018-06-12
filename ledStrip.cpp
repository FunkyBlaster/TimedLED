/*
 * ledStrip.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: Aztec
 */
#include "ledStrip.h"

#include <basictypes.h>
#include <dspi.h>
#include "led.h"
#include <pins.h>
#include <pinconstant.h>
#include <stdio.h>

LedStrip * LedStrip::currentStripSPI = NULL;

LedStrip::LedStrip() {
	J2[25].function( PINJ2_25_DSPI1_SCK );
	J2[27].function( PINJ2_27_DSPI1_SIN );
	J2[28].function( PINJ2_28_DSPI1_SOUT );

	DSPIInit( DEFAULT_DSPI_MODULE, 2000000, 0x8, 0x0F, 0x0F, 0, 0, TRUE, 0, 0 );
}

LedStrip::~LedStrip() {
	delete currentStripSPI;
}

void LedStrip::WriteToDSPI( PBYTE bytePtr, int numBytes ) {
	DSPIStart( DEFAULT_DSPI_MODULE, bytePtr, NULL, numBytes, NULL);
	while( !DSPIdone( DEFAULT_DSPI_MODULE )) {}
}

LedStrip * LedStrip::GetLedStrip() {
	if( currentStripSPI == NULL ) {
		currentStripSPI = new LedStrip();
	}
	return currentStripSPI;
}

BOOL LedStrip::initLedStrip() {
	if( currentStripSPI == NULL ) {
		return false;
	}

	currentStripSPI->WriteToDSPI( clearBits , 2);

	return true;
}

void LedStrip::setLedValue(int i, BYTE r, BYTE g, BYTE b) {
	ledStrip[i].setColorValue(r,g,b);
}

void LedStrip::setStripColor(BYTE r, BYTE g, BYTE b) {
	for( int i = 0; i < ledCount; i++ ) {
		ledStrip[i].setColorValue(r,g,b);
	}
}

void LedStrip::writeLedStrip() {
	for( int i = 0; i < ledCount; i++ ) {
		ledStrip[i].writeLedValues();
		currentStripSPI->WriteToDSPI( clearBits, 2 );
	}
}

