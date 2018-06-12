/*
 * led.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: Aztec
 */
#include "led.h"
#include "ledStrip.h"

#include <stdio.h>

static LedStrip * strip;

/*********************************************
 * Constructor for Led instance              *
 *********************************************/
Led::Led() {
	strip = strip->GetLedStrip();
}

/*********************************************
 * Destructor for Led instance               *
 *********************************************/
Led::~Led() {

}

/***********************************************
 * Sets the color values of this individual LED
 *
 * @param r - red value
 * @param g - green value
 * @param b - blue value
 ***********************************************/
void Led::setColorValue( BYTE r, BYTE g, BYTE b ) {
	redVal = r;
	greenVal = g;
	blueVal = b;
}

/*************************************************
 * Sets the values of this individual LED to white
 *************************************************/
void Led::setColorlessValue() {
	redVal = 127;
	greenVal = 127;
	blueVal = 127;
	iprintf("Set to white.\r\n");
}

/**************************************
 * Writes color values to SPI
 **************************************/
void Led::writeLedValues() {
	BYTE colors[3] = { greenVal, redVal, blueVal };
	strip->WriteToDSPI( colors, 3 );
}

BYTE Led::getColorValues(BYTE id) {
	switch(id) {
	case 0:
		return greenVal;
	case 1:
		return redVal;
	case 2:
		return blueVal;
	}
	return 0;
}
