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
#ifndef MAIN_H_
#define MAIN_H_

char* getCurSysTimeASCII(int fd);

char* getCurStartTimeASCII(int fd);

char* getCurEndTimeASCII(int fd);

char* getTimeZoneASCII(int fd);

void setCurStartTime(int hours, int min, int ampm);

void setCurEndTime(int hours, int min, int ampm);

void setTimeZone(char * tz, char * tzASCII);

char* SerializeClockData(int fd);

void RegisterPost();

BOOL syncSystemTimeNTP();

int timeObjEval(struct tm * one, struct tm * two);

#endif /* MAIN_H_ */
