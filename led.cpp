/*
 * led.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: Aztec
 */

#include "led.h"

#include <stdio.h>
#include <basictypes.h>

#include "ledStrip.h"

extern LedStrip currentStripSPI;

void Led::setColorValue( BYTE r, BYTE g, BYTE b ) {
	redVal = r;
	blueVal = g;
	greenVal = b;
}

void Led::setLedValueCurrentColor(int i) {

}

void Led::writeLedValues() {
	BYTE colors[3] = { redVal, greenVal, blueVal };
	currentStripSPI.WriteToDSPI( colors, 3 );
}
