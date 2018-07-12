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

/***********************************************************************************
 * This class keeps track of the system time, start time,
 * end time, and time zone and their associated ASCII representations.
 * In addition, this class also handles NTP server and on-board
 * RTC synchronization, data serialization for the dynamic webpage,
 * and determining if LEDs should be active or not.
 ***********************************************************************************/
#include <autoupdate.h>
#include <ctype.h>
#include <dhcpclient.h>
#include <dspi.h>
#include <nbtime.h>
#include <NetworkDebug.h>
#include <pins.h>
#include <predef.h>
#include <rtc.h>
#include <smarttrap.h>
#include <startnet.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#include "main.h"
#include "ledStrip.h"

#define LESS_THAN 1
#define EQUAL_TO 2
#define GREATER_THAN 3
#define SEC_PER_WEEK 604800
#define SEC_PER_HOUR 3600
#define ACTIVE 1
#define INACTIVE 0

time_t currentSysTime;
struct tm currentSysTimeStructLocal;
struct tm currentStartTimeStruct;
struct tm currentEndTimeStruct;

const char * AppName="Time-Activated LEDs";
char * timeZoneASCII;
char * lastTimeZoneSet;
char timeBuf[80];
char serialBuf[80];
const char syncBuf[21] = " (Time Sync Failed)\0";

//UserMain() ticks every ~1s
int NTPSyncCounter;
int NTPSyncFailCount;
int RTCSyncCounter;
BOOL sysTimeOutOfSync;
BOOL NTPSyncSuccessful;
BOOL RTCFromSystemSetSuccessful;
BOOL SystemFromRTCSetFailure;

extern const int ledCount;
LedStrip *strip;

extern "C" {
	void UserMain(void * pd);
}

/**********************************************************
 * @brief Get ASCII representation of stored system time
 *
 * @param fd - handle to the network socket connection
 *
 * @return - ASCII string of system time
 **********************************************************/
char * getCurSysTimeASCII(int fd) {
	/*
	 * Clear the buffer used for storing ASCII time strings,
	 * then format the current system time to hour:min ampm
	 * format and return pointer to buffer
	 */
	memset(&timeBuf, 0, 80);
	strftime(timeBuf,80,"%I:%M:%S %p",&currentSysTimeStructLocal);
	// 02:35 -> 2:35
	if(timeBuf[0] == '0') {
		for(int i = 0; i < 79; i++) {
			timeBuf[i] = timeBuf[i+1];
		}
	}
	/*
	 * If system time is out of sync, append
	 * "out of sync message" to end of string
	 */
	if( sysTimeOutOfSync )  {
		for(int i = 0; i < 79; i++) {
			//when we hit the null terminator
			if( timeBuf[i] == 0 ) {
				for(int x = 0; x < 21; x++) {
					timeBuf[i] = syncBuf[x];
					i++;
				}
				i = 79;
			}
		}
	}
	return timeBuf;
}

/*********************************************************
 * @brief Get ASCII representation of stored start time
 *
 * @param fd - handle to the network socket connection
 *
 * @return - ASCII string of start time
 *********************************************************/
char * getCurStartTimeASCII(int fd) {
	//Same as current system function, but for start time
	memset(&timeBuf, 0, 80);
	strftime(timeBuf,80,"%I:%M%p",&currentStartTimeStruct);
	// 02:35 -> 2:35
	if(timeBuf[0] == '0') {
		for(int i = 0; i < 79; i++) {
			timeBuf[i] = timeBuf[i+1];
		}
	}
	return timeBuf;
}

/*******************************************************
 * @brief Get ASCII representation of stored end time
 *
 * @param fd - handle to the network socket connection
 *
 * @return - ASCII string of end time
 *******************************************************/
char * getCurEndTimeASCII(int fd) {
	//Same as current system function, but for end time
	memset(&timeBuf, 0, 80);
	strftime(timeBuf,80,"%I:%M%p",&currentEndTimeStruct);
	// 02:35 -> 2:35
	if(timeBuf[0] == '0') {
		for(int i = 0; i < 79; i++) {
			timeBuf[i] = timeBuf[i+1];
		}
	}
	return timeBuf;
}

/*******************************************************
 * @brief Get ASCII representation of stored time zone
 *
 * @param fd - handle to the network socket connection
 *
 * @return - ASCII string of time zone
 *******************************************************/
char * getCurTimeZoneASCII(int fd) {
	if( timeZoneASCII != 0 ) {
		return timeZoneASCII;
	}
	else {
		return "Time Zone not set, defaulting to UTC/GMT.";
	}
}

/************************************************************
 * @brief Set the desired start (LED=ON) time
 *
 * @param fd - handle to the network socket connection
 * @param hours - The hour value of the new time
 * @param min - The minute value of the new time
 * @param ampm - AM/PM flag (0 = AM, 1 = PM)
 ************************************************************/
void setCurStartTime(int hours, int min, int ampm) {
	/*
	 * 61 is null value set by formatData() if a value was not
	 * changed (if value not changed, don't update start time)
	 */
	if( hours != 61 || min != 61 ) {
		/*
		 * if ampm is not input by user (value passed = 61),
		 * then default to AM; if PM, add 12 to hours (1pm = 1300)
		 */
		if( ampm == 1 ) hours+=12;
		/*
		 * Times cannot be equal, so if they are,
		 * increment end time by one minute
		 */
		struct tm * end = &currentEndTimeStruct;
		struct tm * start = &currentStartTimeStruct;
		if( hours == end->tm_hour ) {
			if( min == end->tm_min ) {
				if( min != 59 ) end->tm_min++;
				else if( hours != 23 ) {
					end->tm_hour++;
					end->tm_min = 0;
					end->tm_mday = 0;
				}
				else {
					end->tm_hour = 0;
					end->tm_min = 0;
				}
			}
		}
		start->tm_hour = hours;
		start->tm_min = min;
		start->tm_sec   = 0;
		start->tm_mday  = 0;
		start->tm_year  = 0;
		start->tm_yday  = 0;
		if( hours > end->tm_hour ) {
			end->tm_mday = 1;
		}
		else if( hours == end->tm_hour ) {
			if( min > end->tm_min ) {
				end->tm_mday = 1;
			}
		}
	}
}

/********************************************************
 * @brief Set the desired end (LED=OFF) time
 *
 * @param fd - handle to the network socket connection
 * @param hours - The hour value of the new time
 * @param min - The minute value of the new time
 * @param ampm - AM/PM flag (0 = AM, 1 = PM)
 ********************************************************/
void setCurEndTime(int hours, int min, int ampm) {
	/*
	 * 61 is null value set by formatData() if a value was not
	 * changed (if value not changed, don't update end time)
	 */
	if( hours != 61 || min != 61 ) {
		/*
		 * if ampm is not input by user (value passed = 61),
		 * then default to AM; if PM, add 12 to hours (1pm = 1300)
		 */
		if( ampm == 1 ) hours+=12;
		/*
		 * Times cannot be equal, so if they are,
		 * increment end time by one minute
		 */
		struct tm * start = &currentStartTimeStruct;
		struct tm * end = &currentEndTimeStruct;
		if( hours == start->tm_hour ) {
			if( min == start->tm_min ) {
				if( min != 59 ) min++;
				else if( hours != 23 ) {
					hours++;
					min = 0;
					end->tm_mday = 0;
				}
				else {
					hours = 0;
					min = 0;
				}
			}
		}
		end->tm_hour = hours;
		end->tm_min = min;
		end->tm_sec  = 0;
		end->tm_mday = 0;
		end->tm_year = 0;
		end->tm_yday = 0;
		if( hours < start->tm_hour ) {
			end->tm_mday = 1;
		}
		else if( hours == start->tm_hour ) {
			if(min < start->tm_min) {
				end->tm_mday = 1;
			}
		}
	}
}

/************************************************************
 * @brief Called with input from POST form, sets
 *        the tz variable and its ASCII equivalent
 *
 * @param fd - handle to the network socket connection
 * @param tz - pointer to tz string literal
 * @param tzASCII - pointer to ASCII representation of tz
 ************************************************************/
void setTimeZone(char * tz, char * tzASCII) {
	tzsetchar(tz);
	timeZoneASCII = tzASCII;
	lastTimeZoneSet = tz;
}

/*******************************************************************
 * @brief Method to be called by clockData.html
 *
 * @param fd - handle to the network socket connection
 *
 * @return - buffer of current system time in ASCII form
 *******************************************************************/
char * SerializeClockData(int fd) {
	memset(&serialBuf, 0, 80);
    snprintf(serialBuf, 80, "%s\r\n", getCurSysTimeASCII(fd));
    return serialBuf;
}

void RegisterPost();

/*********************************************************
 * @brief Sync the system time with the NTP server pool
 *
 * @return - TRUE on success, FALSE on fail
 *********************************************************/
BOOL syncSystemTimeNTP() {

	BOOL retVal = SetTimeNTPFromPool();
	currentSysTime = time(0);
	return retVal;
}

/**********************************************************
 * @brief Evaluate two time structures
 *
 * @param one - pointer to first time struct to compare
 * @param two - pointer to second time struct to compare
 *
 * @return - (one) ___ (two)
 *            0 - null;
 *            1 - less than;
 *            2 - equal to;
 *            3 - greater than;
 **********************************************************/
int timeObjEval(struct tm * one, struct tm * two) {
	int oneMin  = one->tm_min;
	int oneHour = one->tm_hour;
	int twoMin  = two->tm_min;
	int twoHour = two->tm_hour;
	int twoDay = two->tm_mday;
	if( oneHour > twoHour ) {
		if( twoDay == 0 ) return 3;
		else return 1;
	}
	else if( oneHour < twoHour ) {
		if( twoDay == 0 ) return 1;
		else return 3;
	}
	else if( oneHour == twoHour ) {
		if( oneMin > twoMin ) {
			if( twoDay == 0 ) return 3;
			else return 1;
		}
		else if( oneMin < twoMin ) return 1;
		else return 2;
	}
	else return 0;
}

void UserMain(void * pd) {
    InitializeStack();
    GetDHCPAddressIfNecessary();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();
    EnableSmartTraps();
    StartHTTP();
    RegisterPost();

    //Initialize the LED strip
    strip = strip->GetLedStrip();
    strip->initLedStrip();
    strip->turnStripOff();
    strip->updateLedStrip();

    BOOL LEDCurrentState = INACTIVE;
    BOOL LEDPreviousState = INACTIVE;

    //Initialize all of the time variables to non-null values
    currentSysTimeStructLocal = *gmtime(&currentSysTime);
    currentStartTimeStruct = *gmtime(&currentSysTime);
    currentEndTimeStruct = *gmtime(&currentSysTime);

    NTPSyncCounter = SEC_PER_WEEK;
    NTPSyncFailCount = 0;
    RTCSyncCounter = SEC_PER_HOUR;

    while( 1 ) {
    	if( NTPSyncCounter >= SEC_PER_WEEK ) {
    		//Sync RTC to NTP server pool once a month
    		NTPSyncSuccessful = syncSystemTimeNTP();
    		
    		//Only change the RTC if the system time is accurate
    		if( NTPSyncSuccessful ) {
    			NTPSyncCounter = 0;
    			NTPSyncFailCount = 0;
    			RTCFromSystemSetSuccessful = RTCSetRTCfromSystemTime();
    		}
    		else {
    			//If NTP fails 5 times in a row, stop trying to resync until scheduled sync
    			if( NTPSyncFailCount < 5 ) {
    				//If NTP sync fails, try again in 10 sec
    				NTPSyncCounter = SEC_PER_WEEK - 10;
    				NTPSyncFailCount++;
    			}
    		}
    	}
    	if( RTCSyncCounter >= SEC_PER_HOUR ) {
    		//Once an hour, sync the system time to the RTC
    		tzsetchar("GMT0");
    		//Set the system time from the Real-Time Clock
    		SystemFromRTCSetFailure = RTCSetSystemFromRTCTime();
    		if( lastTimeZoneSet != NULL ) {
    			tzsetchar(lastTimeZoneSet);
    		}
    		//iprintf("TZ var: %s\r\n",lastTimeZoneSet);
    		RTCSyncCounter = 0;
    		//If RTC sync fails, try again in 10 sec
    		if( SystemFromRTCSetFailure ) {
    			RTCSyncCounter = SEC_PER_HOUR - 10;
    		}
    	}
    	/*
    	 * Sleep for one second - loop runs approx. every second. Can
    	 * divide this by two to update twice every second, multiply by two
    	 * to update every two seconds, etc.
    	 */
    	OSTimeDly(TICKS_PER_SECOND);
    	/*
    	 * If setting of RTC and system time from NTP pool is successful,
    	 * update HTML time variable and check for time match
    	 */
    	if( NTPSyncSuccessful && !RTCFromSystemSetSuccessful ) {
    		sysTimeOutOfSync = FALSE;
    	}
    	else {
    		sysTimeOutOfSync = TRUE;
    	}

    	//Set currentSysTime variable to the system's time
    	time(&currentSysTime);
    	//Convert to local timezone
    	currentSysTimeStructLocal = *localtime(&currentSysTime);

    	struct tm * sys = &currentSysTimeStructLocal;
    	struct tm * s = &currentStartTimeStruct;
    	struct tm * e = &currentEndTimeStruct;

//    	iprintf("System Time: %s\r\n",asctime(sys));
//    	iprintf("Start Time: %s\r\n",asctime(s));
//    	iprintf("End Time: %s\r\n--------------------------------------\r\n",asctime(e));

    	//IF( system time >= start time )
    	if( timeObjEval(sys,s) == GREATER_THAN || timeObjEval(sys,s) == EQUAL_TO ) {
    		//IF ( system time < end time )
    		if( timeObjEval(sys,e) == LESS_THAN ) {
    			LEDCurrentState = ACTIVE;
    		}
    		//IF( system time >= end time )
    		else {
    			LEDCurrentState = INACTIVE;
    		}
    	}
    	//IF( system time < start time )
    	else {
    		LEDCurrentState = INACTIVE;
    	}

    	/*
    	 * This is to prevent the strip from rewriting colors
    	 * every second; it will only write upon a change
    	 * of state.
    	 */
    	if( LEDCurrentState == ACTIVE ) {
    		//Turn LEDs ON
    		if( LEDPreviousState == INACTIVE ) {
    			strip->setStripWhite();
    			strip->updateLedStrip();
    		}
    	}
    	else {
    		//Turn LEDs OFF
    		if( LEDPreviousState == ACTIVE ) {
    			strip->turnStripOff();
    			strip->updateLedStrip();
    		}
    	}

    	LEDPreviousState = LEDCurrentState;

    	NTPSyncCounter++;
    	RTCSyncCounter++;
    }
}
