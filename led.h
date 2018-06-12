/*
 * led.h
 *
 *  Created on: Jun 11, 2018
 *      Author: Aztec
 */

#ifndef LED_H_
#define LED_H_
#pragma once

#include <basictypes.h>

class Led {
public:
	Led();
	~Led();
	void setColorValue(BYTE r, BYTE g, BYTE b);
	void setColorlessValue();
	void setLedColorValue(int i , BYTE r, BYTE g, BYTE b);
	void writeLedValues();
	BYTE getColorValues(BYTE id);

private:
	BYTE redVal;
	BYTE greenVal;
	BYTE blueVal;
};

#endif /* LED_H_ */
