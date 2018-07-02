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
#ifndef LED_H_
#define LED_H_

#include <basictypes.h>


class Led {
public:
	Led();
	~Led();
	void setColorValue(uint8_t r, uint8_t g, uint8_t b);
	void setColorlessValue();
	void modifyBrightness(uint8_t brightness);
	void writeLedValues();
	void setLedOff();
	int getColorValues(uint8_t id);

private:
	uint8_t redVal;
	uint8_t greenVal;
	uint8_t blueVal;
};

#endif /* LED_H_ */
