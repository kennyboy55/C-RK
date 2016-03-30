#ifndef _RTC_H_
#define	_RTC_H_

#include <time.h>           // for tm-struct

tm gmt;
int synced;

extern void GetDateString(char*);
extern void GetDateTimeString(char*);
extern void GetTimeString(char*);
extern tm* GetTimeStruct(void);
extern int SyncTime( void );
extern int GetTimeSynced( void );

extern int GetTimeZoneValue(int);
extern void GetTimeZoneName(int, char *);

/* Prototypes */
extern int X12Init(void);

extern int X12RtcGetClock(tm *tm);
extern int X12RtcSetClock(CONST tm *tm);
extern int X12RtcSyncClock(void);
extern int X12RtcWrite(int nv, CONST u_char *buff, size_t len);

/* End of prototypes */
#endif
