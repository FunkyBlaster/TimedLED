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
	Led();
	~Led();
	void setColorValue(BYTE r, BYTE g, BYTE b);
	void setColorlessValue();
	void writeLedValues();
	void setLedOff();
	int getColorValues(BYTE id);

private:
	BYTE redVal;
	BYTE greenVal;
	BYTE blueVal;
};

#endif /* LED_H_ */
