/******************************************************************************
* Copyright 1998-2016 NetBurner, Inc.  ALL RIGHTS RESERVED
*
*    Permission is hereby granted to purchasers of NetBurner Hardware to use or
*    modify this computer program for any use as long as the resultant program
*    is only executed on NetBurner provided hardware.
*
*    No other rights to use this program or its derivatives in part or in
*    whole are granted.
*
*    It may be possible to license this or other NetBurner software for use on
*    non-NetBurner Hardware. Contact sales@Netburner.com for more information.
*
*    NetBurner makes no representation or warranties with respect to the
*    performance of this computer program, and specifically disclaims any
*    responsibility for any damages, special or consequential, connected with
*    the use of this program.
*
* NetBurner
* 5405 Morehouse Dr.
* San Diego, CA 92121
* www.netburner.com
******************************************************************************/
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
	void setStripColor(uint8_t r, uint8_t g, uint8_t b);
	void setStripWhite();
	void modifyStripBrightness(uint8_t brightPercent);
	void setLedValue(int i, uint8_t r, uint8_t g, uint8_t b);
	void updateLedStrip();
	void turnStripOff();

private:
	uint8_t clearBytes[];
	Led ledStrip[ledCount];
	static LedStrip *currentStripSPI;
};

#endif /* LEDSTRIP_H_ */
