/*
 * led.h
 *
 *  Created on: Jun 11, 2018
 *      Author: Aztec
 */

#ifndef LED_H_
#define LED_H_

#include <basictypes.h>

class Led {
public:
	void setColorValue(BYTE r, BYTE g, BYTE b);
	void setLedValueCurrentColor(int i);
	void setLedValue(int i , BYTE r, BYTE g, BYTE b);
	void writeLedValues();

private:
	BYTE redVal;
	BYTE greenVal;
	BYTE blueVal;
};

#endif /* LED_H_ */
