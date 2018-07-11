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

/********************************************************
 * This header file establishes which functions
 * index.htm needs to have access to. Nearly all
 * of said functions relate to retrieving ASCII
 * strings of updated data.
 ********************************************************/
#ifndef HTMLVARS_H_
#define HTMLVARS_H_

#include <constants.h>
#include <system.h>
#include <startnet.h>
#include <time.h>

char* getCurSysTimeASCII(int fd);

char* getCurStartTimeASCII(int fd);

char* getCurEndTimeASCII(int fd);

char* getCurTimeZoneASCII(int fd);

char* SerializeClockData(int fd);

void setCurStartTime(int fd, int hours, int min, int ampm);

void setCurEndTime(int fd, int hours, int min, int ampm);

#endif /*HTMLVARS_H_*/
