/*
 * This class keeps track of the system time, start time,
 * end time, and time zone and their associated ASCII representations.
 * In addition, this class also handles NTP server and on-board
 * RTC synchronization, data serialization for the dynamic webpage,
 * and determining if LEDs should be active or not.
 */
#include <autoupdate.h>
#include <cstring>
#include <ctype.h>
#include <dhcpclient.h>
#include <dspi.h>
#include <nbtime.h>
#include <NetworkDebug.h>
#include <pins.h>
#include <predef.h>
#include <rtc.h>
#include <smarttrap.h>
#include <sstream>
#include <startnet.h>
#include <stdio.h>
#include <time.h>

#include "main.h"
#include "ledStrip.h"

#define TICKS_PER_MONTH 2592000;
#define TICKS_PER_HOUR 3600;

time_t currentSysTime;
struct tm currentSysTimeStructGMT;
struct tm currentSysTimeStructLocal;
struct tm currentStartTimeStruct;
struct tm currentEndTimeStruct;
struct tm compareTimeStruct;

const char * AppName="Time-Activated LEDs";
char * timeZoneASCII;
char * prevTimeZone;
char timeBuf[80];
char serialBuf[80];
const char syncBuf[21] = " (Time Sync Failed)\0";
const int ticksInOneMonth = TICKS_PER_MONTH;
const int ticksInOneHour  = TICKS_PER_HOUR;
int NTPSyncCounter= ticksInOneMonth;
int RTCSyncCounter= ticksInOneHour;
extern const int ledCount;
LedStrip *strip;

BOOL sysTimeOutOfSync;
BOOL LEDsPowered;
BOOL NTPSyncSuccessful;
BOOL RTCFromSystemSetSuccessful;
BOOL SystemFromRTCSetSuccessful;

extern "C" {
	void UserMain(void * pd);
}

/**********************************************************
 * @brief Get ASCII representation of stored system time  *
 *                                                        *
 * @param fd - handle to the network socket connection    *
 *                                                        *
 * @return - ASCII string of system time                  *
 **********************************************************/
char * getCurSysTimeASCII(int fd) {
	/*
	 * Clear the buffer used for storing ASCII time strings,
	 * then format the current system time to hour:min ampm
	 * format and return pointer to buffer
	 */
	memset(&timeBuf, 0, 80);
	strftime(timeBuf,80,"%I:%M %p",&currentSysTimeStructLocal);
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
 * @brief Get ASCII representation of stored start time  *
 *                                                       *
 * @param fd - handle to the network socket connection   *
 *                                                       *
 * @return - ASCII string of start time                  *
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
 * @brief Get ASCII representation of stored end time  *
 *                                                     *
 * @param fd - handle to the network socket connection *
 *                                                     *
 * @return - ASCII string of end time                  *
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
 * @brief Get ASCII representation of stored time zone *
 *                                                     *
 * @param fd - handle to the network socket connection *
 *                                                     *
 * @return - ASCII string of time zone                 *
 *******************************************************/
char * getCurTimeZoneASCII(int fd) {
	if( timeZoneASCII != 0 ) {
		return timeZoneASCII;
	}
	else {
		return "Time Zone not set. Defaulting to UTC (GMT).";
	}
}

/*******************************************************
 * @brief Set the desired start (LED=ON) time          *
 *                                                     *
 * @param fd - handle to the network socket connection *
 * @param hours - The hour value of the new time       *
 * @param min - The minute value of the new time       *
 * @param ampm - AM/PM flag (0 = AM, 1 = PM)           *
 *******************************************************/
void setCurStartTime(int fd, int hours, int min, int ampm) {
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
		if( hours == end->tm_hour ) {
			if( min == end->tm_min ) {
				if( min != 59 ) end->tm_min++;
				else if( hours != 23 ) {
					end->tm_hour++;
					end->tm_min = 0;
				}
				else {
					end->tm_hour = 0;
					end->tm_min = 0;
				}
			}
		}
		currentStartTimeStruct.tm_hour = hours;
		currentStartTimeStruct.tm_min = min;
		currentStartTimeStruct.tm_sec   = 0;
		currentStartTimeStruct.tm_mday  = 0;
		currentStartTimeStruct.tm_year  = 0;
		currentStartTimeStruct.tm_yday  = 0;
		if( hours > end->tm_hour ) {
			end->tm_mday = 1;
		}
	}
}

/********************************************************
 * @brief Set the desired end (LED=OFF) time            *
 *                                                      *
 * @param fd - handle to the network socket connection  *
 * @param hours - The hour value of the new time        *
 * @param min - The minute value of the new time        *
 * @param ampm - AM/PM flag (0 = AM, 1 = PM)            *
 ********************************************************/
void setCurEndTime(int fd, int hours, int min, int ampm) {
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
		if( hours == start->tm_hour ) {
			if( min == start->tm_min ) {
				if( min != 59 ) min++;
				else if( hours != 23 ) {
					hours++;
					min = 0;
				}
				else {
					hours = 0;
					min = 0;
				}
			}
		}
		currentEndTimeStruct.tm_hour = hours;
		currentEndTimeStruct.tm_min = min;
		currentEndTimeStruct.tm_sec  = 0;
		currentEndTimeStruct.tm_mday = 0;
		currentEndTimeStruct.tm_year = 0;
		currentEndTimeStruct.tm_yday = 0;
		if( hours < start->tm_hour ) {
			currentEndTimeStruct.tm_mday = 1;
		}
	}
}

/************************************************************
 * @brief Called with input from POST form, sets            *
 *        the tz variable and its ASCII equivalent          *
 *                                                          *
 * @param fd - handle to the network socket connection      *
 * @param tz - pointer to tz string literal                 *
 * @param tzASCII - pointer to ASCII representation of tz   *
 ************************************************************/
void setTimeZone(int fd, char * tz, char * tzASCII) {
	tzsetchar(tz);
	timeZoneASCII = tzASCII;
	prevTimeZone = tz;
}

/*********************************************************
 * @brief Method to be called by clockData.html          *
 *                                                       *
 * @param fd - handle to the network socket connection   *
 *                                                       *
 * @return - buffer of current system time in ASCII form *
 *********************************************************/
char * SerializeClockData(int fd)
{
	memset(&serialBuf, 0, 80);
    snprintf(serialBuf, 80, "%s\r\n", getCurSysTimeASCII(fd));
    return serialBuf;
}

void RegisterPost();

/*********************************************************
 * @brief Sync the system time with the NTP server pool  *
 *                                                       *
 * @return - TRUE on success, FALSE on fail              *
 *********************************************************/
BOOL SyncSystemTimeNTP() {

	BOOL retVal = SetTimeNTPFromPool();
	currentSysTime = time(0);
	return retVal;
}

/**********************************************************
 * @brief Accuracy to the second not important;           *
 *                                                        *
 * @param one - pointer to first time struct to compare   *
 * @param two - pointer to second time struct to compare  *
 *                                                        *
 * @return - (one) ___ (two)                              *
 *            0 - null;                                   *
 *            1 - less than;                              *
 *            2 - equal to;                               *
 *            3 - greater than;                           *
 **********************************************************/
int timeObjEval(struct tm * one, struct tm * two) {
	int oneMin  = one->tm_min;
	int oneHour = one->tm_hour;
	int twoMin  = two->tm_min;
	int twoHour = two->tm_hour;
	int twoDay = two->tm_mday;
	if( oneHour > twoHour ) {
		if( twoDay == 1 ) return 1;
		else return 3;
	}
	else if( oneHour < twoHour ) {
		if( twoDay == 0 ) return 3;
		else return 1;
	}
	else if( oneHour == twoHour ) {
		if( oneMin > twoMin ) return 3;
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

    LEDsPowered = FALSE;

    //Initialize all of the time variables to non-null values
    currentSysTimeStructGMT = *localtime(&currentSysTime);
    currentSysTimeStructLocal = *gmtime(&currentSysTime);
    currentStartTimeStruct = currentSysTimeStructGMT;
    currentEndTimeStruct = currentSysTimeStructGMT;

    while( 1 ) {
    	if( NTPSyncCounter >= ticksInOneMonth || sysTimeOutOfSync == TRUE ) {
    		//Sync RTC to NTP server pool once a month
    		NTPSyncSuccessful = SyncSystemTimeNTP();
    		RTCFromSystemSetSuccessful = RTCSetRTCfromSystemTime();
    		NTPSyncCounter = 0;
    	}
    	if( RTCSyncCounter >= ticksInOneHour || sysTimeOutOfSync == TRUE ) {
    		//Once an hour, sync the system time to the RTC
    		SystemFromRTCSetSuccessful = RTCSetSystemFromRTCTime();
    		RTCSyncCounter = 0;
    	}

    	OSTimeDly(TICKS_PER_SECOND);
    	/*
    	 * If setting of RTC and system time from NTP pool is successful,
    	 * update HTML time variable and check for time match
    	 */
    	if( NTPSyncSuccessful == TRUE && RTCFromSystemSetSuccessful == 0 ) {

    		currentSysTime = time(0);
    		gmtime_r(&currentSysTime, &currentSysTimeStructGMT);
    		localtime_r(&currentSysTime, &currentSysTimeStructLocal);
    		struct tm * sys = &currentSysTimeStructLocal;
    		struct tm * s = &currentStartTimeStruct;
    		struct tm * e = &currentEndTimeStruct;
    		//IF( system time >= start time)
    		if( timeObjEval(sys,s) == 3 || timeObjEval(sys,s) == 2 ) {
    			//IF ( system time < end time )
    			if( timeObjEval(sys,e) == 1 ) {
    				LEDsPowered = TRUE;
    			}
    			else {
    				LEDsPowered = FALSE;
    			}
    		}
    		else {
    			LEDsPowered = FALSE;
    		}
    		if( LEDsPowered == TRUE ) {
    			strip->setStripWhite();
    		}
    		else {
    			strip->turnStripOff();
    		}

    	}
    	else sysTimeOutOfSync = TRUE;

    	NTPSyncCounter++;
    	RTCSyncCounter++;
    }
}
