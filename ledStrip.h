/*
 * ledStrip.h
 *
 *  Created on: Jun 11, 2018
 *      Author: Aztec
 */

#ifndef LEDSTRIP_H_
#define LEDSTRIP_H_

#include <basictypes.h>

#include "led.h"

const int ledCount = 64;

class LedStrip {
public:
	LedStrip();
	~LedStrip();
	static LedStrip * GetLedStrip();
	void WriteToDSPI(PBYTE bytePtr, int numBytes);
	BOOL initLedStrip();

private:
	static LedStrip * currentStripSPI;
	void setLedValue(int i, BYTE r, BYTE g, BYTE b);
	void setStripColor(BYTE r, BYTE g, BYTE b);
	void writeLedStrip();
	BYTE clearBits[2] = {0,0};
	Led ledStrip[ledCount];
};

#endif /* LEDSTRIP_H_ */
