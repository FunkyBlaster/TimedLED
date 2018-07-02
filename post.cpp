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

/**************************************************************
 * This class handles all the data input by the
 * user and submitted via a POST form. It formats
 * the data and passes it to various functions
 * in main.cpp.
 **************************************************************/
#include <basictypes.h>
#include <htmlfiles.h>
#include <http.h>
#include <iosys.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"

#define MAX_BUF_LEN 80
#define NULL_VAL 61

char startForm[MAX_BUF_LEN];

/*
 * desiredTimeInput Indexes:
 * 0 - startHr, 1 - startMin, 2 - startAmPm
 * 3 - endHr,   4 - endMin,   5 - endAmPm, 6 - time zone
 */
int desiredTimeInput[7];

extern "C" {
void writeStartForm( int sock, PCSTR url );
}

void writeStartForm( int sock, PCSTR url ) {
	writestring( sock, startForm );
}

/********************************************
 * @brief Pass time zone to main.cpp        *
 *                                          *
 * @param sock - handle to network socket   *
 * @param input - time zone string          *
 ********************************************/
void tzToInt( std::string input ) {
	/*
	 * input variable comes from POST timezone selection;
	 * pass as tz variable to setTimeZone in main.cpp along
	 * with representative ASCII string
	 */
	if( input == "PST8PDT7" ) {
		setTimeZone("PST8PDT7","Pacific Time (US)");
	}
	else if( input == "MST7MDT6") {
		setTimeZone("MST7MDT6","Mountain Time (US)");
	}
	else if( input == "CST6CDT5" ) {
		setTimeZone("CST6CDT5","Central Time (US)");
	}
	else if( input == "EST5EDT4" ) {
		setTimeZone("EST5EDT4","Eastern Time (US)");
	}
	else if( input == "AKST9AKDT8" ) {
		setTimeZone("AKST9AKDT8","Alaska Time (US)");
	}
	else if( input == "HST10" ) {
		setTimeZone("HST10","Hawaiian Time (US)");
	}
	else if( input == "AST4ADT3" ) {
		setTimeZone("AST4ADT3","Atlantic Time (US/CAN)");
	}
	else if( input == "WEST-1WET0" ) {
		setTimeZone("WEST-1WET0","Western European Time (EU)");
	}
	else if( input == "CEST-2CET-1" ) {
		setTimeZone("CEST-2CET-1","Central European Time (EU)");
	}
	else if( input == "EEST-3EET-2" ) {
		setTimeZone("EEST-3EET-2","Eastern European Time (EU)");
	}
	else if( input == "MSD-4MSK-3" ) {
		setTimeZone("MSD-4MSK-3","Moscow Time (EU/RU)");
	}
	else if( input == "AWST-8" ) {
		setTimeZone("AWST-8","Australian Western Time (AU)");
	}
	else if( input == "AEDT-11AEST-10" ) {
		setTimeZone("AEDT-11AEST-10","Australian Eastern Time (AU)");
	}
	else if( input == "GMT0" ) {
		setTimeZone("GMT0","Greenwich Mean Time/Universal Coordinated Time/Zulu Time");
	}

}

/*******************************************************************
 * @brief Takes raw data from HTML form and formats                *
 *        it and places it into desiredTimeInput                   *
 *                                                                 *
 * @param fd - handle to network socket                            *
 * @param dataPtr - pointer to raw data, passed by MyDoPost(...)   *
 *******************************************************************/
void formatData(char * dataPtr) {
	//set desiredTimeInput to pre-determined null value (61)
	for(int i = 0; i <= 6; i++) desiredTimeInput[i] = NULL_VAL;
	int fieldId;
	std::string str(dataPtr);
	std::string cvt;
	for(std::string::size_type i = 0; i < str.size(); i++) {
		if( str[i] == '=' ) {
			/*
			 * Data format: field0=[value]&field1=[value]&
			 *
			 * i is the index of a '='
			 * i+1 (and i+2 if two digits) is location of an valid input char
			 * i+2 (or i+3 if two digits) is end char ('&')
			 * i-1 is field ID
			 */
			fieldId = str[i-1] - '0';

			//If data value is AM/PM flag:
			if( fieldId == 2 || fieldId == 5 ) {
				//AMPM flag is = 1 if PM (if first character is 'P')
				if( str[i+1] == 'p' ) {
					desiredTimeInput[fieldId] = 1;
					//iprintf("1 into time input array\r\n");
				}

				else if( str[i+1] == 'a' ) {
					desiredTimeInput[fieldId] = 0;
					//iprintf("0 into time input array\r\n");
				}

				else {
					//Error condition
					desiredTimeInput[fieldId] = 2;
				}
			}
			//If data value is only one character:
			else if( str[i+2] == '&' ) {
				//i+1 = starting digit of data value
				cvt = str.substr(i+1,1);
				desiredTimeInput[fieldId] = atoi(cvt.c_str());
			}
			//If data value is two characters
			else if( str[i+3] == '&' ) {
				cvt = str.substr(i+1,2);
				desiredTimeInput[fieldId] = atoi(cvt.c_str());
			}
			else if( fieldId == 6 ){
				std::string output = str.substr(i+1,std::string::npos);
				tzToInt(output);
				//Get substring from start of tz variable to end of string
			}
		}

	}
}


int myDoPost( int sock, char * url, char * pData, char * rxBuffer ) {
	//Format data and place into desiredTimeInput[]
	formatData(pData);
	//Update start/end times in main.cpp
	setCurStartTime(desiredTimeInput[0],desiredTimeInput[1],desiredTimeInput[2]);
	setCurEndTime(desiredTimeInput[3],desiredTimeInput[4],desiredTimeInput[5]);
	RedirectResponse( sock, "index.htm" );
	return 1;
}

void RegisterPost() {
	SetNewPostHandler( myDoPost );
}
