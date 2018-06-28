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
void Led::setColorValue( uint8_t r, uint8_t g, uint8_t b ) {
	if( r > 127 ) r = 127;
	if( g > 127 ) g = 127;
	if( b > 127 ) b = 127;
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

/******************************************************************
 * @brief Sets the brightness, applies to all 3 color values      *
 *                                                                *
 * @param brightness - percentage brightness (100 - max, 0 - min) *
 ******************************************************************/
void Led::modifyBrightness(uint8_t brightness) {
	//Make sure brighteness isn't OOB
	if( brightness > 100 ) brightness = 100;
	if( brightness < 0 ) brightness = 0;
	/*
	 * AND redVal to ignore the most significant bit, which
	 * gives us the current brightness value.
	 * then compute the new brightness value for each color.
	 * Next, OR redVal to add most significant bit.
	 * Finally, repeat for each color.
	 */
	redVal = redVal & 0x7F;
	redVal = (brightness * redVal) / 100;
	redVal = redVal | 0x80;

	greenVal = greenVal & 0x7F;
	greenVal = (brightness * greenVal) / 100;
	greenVal = greenVal | 0x80;

	blueVal = blueVal & 0x7F;
	blueVal = (brightness * blueVal) / 100;
	blueVal = blueVal | 0x80;
}
/******************************************
 * @brief Writes color values to SPI      *
 *                                        *
 * @note - With this LED strip you have	  *
 *         to push the colors on as GRB,  *
 *         as opposed to RGB - don't fix. *
 ******************************************/
void Led::writeLedValues() {
	uint8_t colors[3] = { greenVal, redVal, blueVal };
	LedStrip::GetLedStrip()->WriteToDSPI( colors, 3 );
}

/********************************************************
 * @brief Returns color values of this LED (debugging)  *
 ********************************************************/
int Led::getColorValues(uint8_t id) {
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
