/* ========================================================================
 * [PROJECT]    SIR
 * [MODULE]     Real Time Clock
 * [TITLE]      High- and low level Routines for INtersil X1205 RTC chip
 * [FILE]       rtc.c
 * [VSN]        1.0
 * [CREATED]    13042007
 * [LASTCHNGD]  131042007
 * [COPYRIGHT]  Copyright (C) STREAMIT BV 2010
 * [PURPOSE]    contains all interface- and low-level routines to
 *              read/write date/time/status strings from the X1205
 * ======================================================================== */

#define LOG_MODULE  LOG_RTC_MODULE

#include <cfg/os.h>
#include <dev/twif.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/thread.h>

#include <arpa/inet.h>
#include <net/route.h>
#include <pro/dhcp.h>
#include <pro/sntp.h>
 
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <time.h>

#include "rtc.h"
#include "portio.h"

//Timezones with offset value and name
const int tz[25] = { -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, +1, +2, +3, +4, +5, +6, +7, +8, +9, +10, +11, +12, +13};
const char tzName[25][30] = { "Midway", "Honolulu", "Anchorage", "Los Angeles", "Phoenix", "Chicago", "New York", "Santiago", "Buenos Aires", "South-Georgia", "Azores", "London", "Amsterdam", "Athens", "Moscow", "Dubai", "Pakistan", "Bangladesh", "Bangkok", "Bejing", "Tokyo", "Sydney", "Solomon", "Auckland", "Samoa" };


//Sync the RTC with NTP server and apply timezone offset
int X12RTCSyncClock(void)
{
	time_t cur_time = 0;
    tm *cur_dt = malloc(sizeof(tm));
	
    uint32_t timeserver = 0;
	
    timeserver = inet_addr("85.254.217.3");
	
	int i;
	//3 attempts are allowed before giving up	
    for (i = 0; i < 3; i++) {
		
		//Try to get the NTP time
        if (NutSNTPGetTime(&timeserver, &cur_time) == 0) {
			synced = 1;	//Mark time as synced
			
			printf("[RTC] Sync Succesfull\n");
			
			//TimeZone
			_timezone = 0;
			stime(&cur_time);
			
			 _timezone = -GetTimeZone() * 60 * 60; //Apply timezone offset
            _daylight = 1;
            time(&cur_time);
            cur_dt = localtime(&cur_time);	//Calculate timezone offset
			
			X12RtcSetClock(cur_dt); //Set the new time in the RTC
            break;
        } else {
			synced = 0;	//Mark time as out of sync
            NutSleep(500);
            printf("[RTC] Sync Failed, retrying\n");
        }
    }
	
	free(cur_dt);
	
	SetConnected(synced);	//Mark network connections the same as synced value
	
	return 1;
}

//Format the RTC value to a Time string
void GetTimeString(char * str)
{
	if (X12RtcGetClock(&gmt) == 0)
    {
		gmt.tm_year = gmt.tm_year + 1900;
		sprintf(str, "%02d:%02d:%02d", gmt.tm_hour, gmt.tm_min, gmt.tm_sec);
    }
}

//Format the RTC value to a Date string
void GetDateString(char * str)
{
	if (X12RtcGetClock(&gmt) == 0)
    {
		gmt.tm_year = gmt.tm_year + 1900;
		sprintf(str, "%02d-%02d-%04d", gmt.tm_mday, gmt.tm_mon+1, gmt.tm_year );
    }
}

//Format the RTC value to a DateTime string
void GetDateTimeString(char * str)
{
	if (X12RtcGetClock(&gmt) == 0)
    {
		gmt.tm_year = gmt.tm_year + 1900;
		sprintf(str, "%02d:%02d %02d-%02d-%04d", gmt.tm_hour, gmt.tm_min, gmt.tm_mday, gmt.tm_mon+1, gmt.tm_year );
    }
}

//Get the time struct which is synced using the RTC and NTP
tm* GetTimeStruct(void)
{
	if (X12RtcGetClock(&gmt) == 0)
    {
		gmt.tm_year = gmt.tm_year + 1900;
    }
	
	return &gmt;
}

//Sync time with the NTP server
int SyncTime( void )
{
	X12RTCSyncClock();
	
	if (X12RtcGetClock(&gmt) == 0)
    {
		//X12RtcGetClockTimezone(&gmt, GetTimeZone());
		gmt.tm_year = gmt.tm_year + 1900;
		printf("[RTC] Time [%02d:%02d:%02d / %02d-%02d-%04d]\n", gmt.tm_hour, gmt.tm_min, gmt.tm_sec, gmt.tm_mday, gmt.tm_mon+1, gmt.tm_year );
    }
	
	return synced;
}

int GetTimeSynced(void)
{
	return synced;
}

int GetTimeZoneValue(int i)
{
	return tz[i];
}

void GetTimeZoneName(int i, char * string)
{
	 strcpy(string, tzName[i]);
}
