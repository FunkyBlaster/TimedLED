/*
 * ledStrip.h
 *
 *  Created on: Jun 11, 2018
 *      Author: Aztec
 */

#ifndef LEDSTRIP_H_
#define LEDSTRIP_H_
#pragma once

#include <basictypes.h>

#include "led.h"

const int ledCount = 64;

class LedStrip {
public:
	LedStrip();
	~LedStrip();
	LedStrip * GetLedStrip();
	void WriteToDSPI(PBYTE bytePtr, int numBytes);
	BOOL initLedStrip();
	void setStripColor(BYTE r, BYTE g, BYTE b);
	void setStripWhite();
	void setLedValue(int i, BYTE r, BYTE g, BYTE b);
	void writeLedStrip();
	void turnStripOff();

private:
	BYTE clearBits[2] = {0,0};
	Led ledStrip[ledCount];
};

#endif /* LEDSTRIP_H_ */
