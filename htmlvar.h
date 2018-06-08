/*
 * This header file establishes which functions
 * index.htm needs to have access to. Nearly all
 * of said functions relate to retrieving ASCII
 * strings of updated data.
 */
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
