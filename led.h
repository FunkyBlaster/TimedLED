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
	void setColorValue(uint8_t r, uint8_t g, uint8_t b);
	void setColorlessValue();
	void modifyBrightness(int brightness);
	void writeLedValues();
	void setLedOff();
	int getColorValues(uint8_t id);

private:
	uint8_t redVal;
	uint8_t greenVal;
	uint8_t blueVal;
};

#endif /* LED_H_ */
