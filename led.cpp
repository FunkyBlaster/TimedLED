/*
 * led.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: Aztec
 */
#include "led.h"
#include "ledStrip.h"

#include <stdio.h>

/****************************************
 * @brief Constructor for Led instance  *
 ****************************************/
Led::Led() {
	redVal = 0;
	greenVal = 0;
	blueVal = 0;
}

/****************************************
 * @brief Destructor for Led instance   *
 ****************************************/
Led::~Led() {
	delete this;
}

/*******************************************************
 * @brief Sets the color values of this individual LED *
 *                                                     *
 * @param r - red value (0-127)                        *
 * @param g - green value (0-127)                      *
 * @param b - blue value (0-127)                       *
 *******************************************************/
void Led::setColorValue( BYTE r, BYTE g, BYTE b ) {
	redVal = r | 0x80;
	greenVal = g | 0x80;
	blueVal = b | 0x80;
}

/**********************************************************
 * @brief Sets the values of this individual LED to white *
 **********************************************************/
void Led::setColorlessValue() {
	redVal = 0x7F;
	greenVal = 0x7F;
	blueVal = 0x7F;
}

/*********************************************************
 * @brief Sets the values of this individual LED to OFF  *
 *********************************************************/
void Led::setLedOff() {
	redVal = 0x80;
	greenVal = 0x80;
	blueVal = 0x80;
}

/******************************************
 * @brief Writes color values to SPI      *
 *                                        *
 * @note - With this LED strip you have	  *
 *         to push the colors on as GRB,  *
 *         as opposed to RGB - don't fix. *
 ******************************************/
void Led::writeLedValues() {
	BYTE colors[3] = { greenVal, redVal, blueVal };
	LedStrip::GetLedStrip()->WriteToDSPI( colors, 3 );
}

/********************************************************
 * @brief Returns color values of this LED (debugging)  *
 ********************************************************/
int Led::getColorValues(BYTE id) {
	switch(id) {
	case 0:
		return (int)greenVal;
	case 1:
		return (int)redVal;
	case 2:
		return (int)blueVal;
	}
	return 0;
}
