#include <stdio.h>
#include "keyboard.h"
#include "display.h"
#include "settings.h"

#include <string.h>
#include <sys/timer.h>
#include <sys/thread.h>
#include <dev/nicrtl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>
#include <net/route.h>
#include <pro/dhcp.h>
#include <pro/sntp.h>

#include "web.h"
#include "vs10xx.h"
#include "rtc.h"
#include "menu.h"
#include "player.h"
#include "streamer.h"
#include "alarm.h"
#include "network.h"
#include "scheduler.h"

#define HOME 8
#define MENU 10
#define STREAMING 10
#define SCHEDULER 11
#define ALARMS 12
#define SETTINGS 13
#define DEVICE 14

int CurrentTimeZone = 11;
int CurrentStream = 0;
int CurrentAlarm = 0;
int CurrentSchedule = 0;
int CurrentScrollPos = 0;
int CurrentAlarmDisplay = 0;
char buf[24] = {0};
char str[30] = {0};
char icon[8] = {0};

/*
	First Time Setup
*/
void SyncTimeEntry(void)
{
	 if(SyncTime())	//If sync succeeded
		navigateToMenu(4);
	else	//if sync failed
		navigateToMenu(5);
}

void CheckFirstTimeSetup(void)
{
	//Check if FTS is set in eeprom, skip FTS
	if(GetFirstTimeSetup())
	{
		navigateToMenu(HOME);
		return;
	}
}

//Mark FTS as done in eeprom
void FirstTimeSetupComplete(void)
{
	SetFirstTimeSetup(1);
}


/*
	TIMEZONES
*/

void DisplayTimeZone(void)
{
	GetTimeZoneName(CurrentTimeZone, str);
	
	sprintf(buf, "~1 %s", str);
	
	LcdLine(1, buf);
}

//Automatically find the timezone using the curent IP address
void FindTimeZone(void)
{
	getTimezoneOffset(&CurrentTimeZone);
	DisplayTimeZone();
}

void ChooseTimeZoneUp(void)
{
	CurrentTimeZone--;
	
	if(CurrentTimeZone < 0)
		CurrentTimeZone = 24;
	
	DisplayTimeZone();
}

void ChooseTimeZoneDown(void)
{
	CurrentTimeZone++;
	
	if(CurrentTimeZone > 24)
		CurrentTimeZone = 0;
	
	DisplayTimeZone();
}

//Sync using the selected timezone
void ChooseTimeZone(void)
{
	SetTimeZone(GetTimeZoneValue(CurrentTimeZone));
}

//Display the time and date in the RTC chip
void DisplayDateTime(void)
{
	GetDateTimeString(str);
	
	LcdLine(0, str);
}


/*
	MAIN DISPLAYS
*/

//Create the icons at the top
void CreateIconsString(char* str, int * len)
{
	int length = 0;
	
	strcpy(str, "");
	
	length++;
	if(GetVolume() == 0)	//Volume on or muted
	{
		strcat(str, "~4");	//muted
	}
	else
	{
		strcat(str, "~3");	//on
	}
	
	if(GetSync() > 0)	//Display checkmark if sync with server is on
	{
		strcat(str, "~0");
		length++;
	}
	
	if(GetTimeSynced())	//Display checkmark if time is in sync with server
	{
		strcat(str, "~7");
		length++;
	}
	
	if(AlarmsOn() || SchedulesOn())	//Display alarm icon if a schedule or alarm is going off
	{
		strcat(str, "~2");
		length++;
	}
	
	*len = length;
}

//Show current stream MetaData and scroll
void DisplayMetaData(void)
{
	if(isStreaming())
	{
		CurrentScrollPos++;
		if(CurrentScrollPos > strlen(GetMetaData()) - 16)
			CurrentScrollPos = 0;
		
		LcdScrollLine(1, CurrentScrollPos, GetMetaData());
	}
	else
	{
		LcdLine(1, " ");
		CurrentScrollPos = 0;
	}
}

//Display the current timestamp followed by the icons
void UpdateTimeDisplay( void )
{
	GetTimeString(str);
	int l;
	CreateIconsString(icon, &l);
	
	int i;
	for(i=0; i<8-l; i++)
		strcat(str, " ");
	
	strcat(str, icon);
	
	LcdLine(0, str);
	
	DisplayMetaData();
}

//Display the current date followed by the icons
void UpdateDateDisplay( void )
{
	GetDateString(str);
	
	int l;
	CreateIconsString(icon, &l);
	
	int i;
	for(i=0; i<6-l; i++)
		strcat(str, " ");
	
	strcat(str, icon);
	
	LcdLine(0, str);
	
	DisplayMetaData();
}


/*
	RESET
*/
void FactoryResetOk(void)
{
	stop();	//Stop any audio	
	resetServer();	//Remove any data on the server
	
	NutSleep(10);
	
	FactoryReset();	//Reset the eeprom memory
	
	NutSleep(20);
	
	Reboot();	//Automatically reboot the device
}


/*
	VOLUME/BASS/TREBLE SETTINGS
*/
void VolumeDisplay(void)
{
	int volume = GetVolume() / 16;
	
	sprintf(buf, "Volume: %d", volume);
	LcdLine(0, buf);
	
	strcpy(str, "");
	int i;
	for(i = 0; i<volume; i++)
	{
		strcat(str, "-");
		NutSleep(2);
	}
	LcdLine(1, str);
}

void VolumeUp(void)
{
	SetVolume(GetVolume() + 16);
	VolumeDisplay();
}
void VolumeDown(void)
{
	SetVolume(GetVolume() - 16);
	VolumeDisplay();
}

void TrebleDisplay(void){
	
	sprintf(buf, "Treble: %d", GetTreble());
	LcdLine(0, buf);
	
	strcpy(str, "");
	int i;
	for(i = 0; i<GetTreble(); i++)
	{
		strcat(str, "-");
		NutSleep(2);
	}
	LcdLine(1, str);
}

void TrebleUp(void){
	SetTreble(GetTreble()+1);
	TrebleDisplay();
}

void TrebleDown(void){
	SetTreble(GetTreble()-1);
	TrebleDisplay();
}

void BassDisplay(void){
	sprintf(buf, "Bass: %d", GetBass());
	LcdLine(0, buf);
	
	strcpy(str, "");
	int i;
	for(i = 0; i<GetBass(); i++)
	{
		strcat(str, "-");
		NutSleep(2);
	}
	LcdLine(1, str);
}

void BassUp(void){
	SetBass(GetBass()+1);
	BassDisplay();
}

void BassDown(void){
	SetBass(GetBass()-1);
	BassDisplay();
}

/*
	DEVICE
*/
void DisplayDeviceID(void)
{
	char str[6];
	GetDeviceID(str);
	
	sprintf(buf, "ID: %s", str);
	
	LcdLine(1, buf);
}


/*
	STREAMS
*/
void DisplayStream(void)
{
	if(GetNumStreams() == 0)	//No streams in memory
	{
		LcdLine(1, "No streams");
		return;
	}
	
	GetStreamName(CurrentStream, str);	//Display the selected stream
	
	sprintf(buf, "~1~6 %s", str);
	
	LcdLine(1, buf);
}

void StreamUp(void)
{
	CurrentStream--;
	
	if(CurrentStream < 0)
		CurrentStream = GetNumStreams() - 1;
	
	DisplayStream();
}

void StreamDown(void)
{
	CurrentStream++;
	
	if(CurrentStream > GetNumStreams() - 1)
		CurrentStream = 0;
	
	DisplayStream();
}

void SelectStream(void)
{
	PlayStream(CurrentStream);
}

void StopStream(void)
{
	stop();	//Stop any stream
}

//Shortcuts to streams in memory
void PlayStream1(void)
{
	PlayStream(0);
}
void PlayStream2(void)
{
	PlayStream(1);
}
void PlayStream3(void)
{
	PlayStream(2);
}
void PlayStream4(void)
{
	PlayStream(3);
}
void PlayStream5(void)
{
	PlayStream(4);
}


/*
	SCHEDULER
*/
void DisplaySchedule(void)
{
	if(GetNumSchedules() == 0)
	{
		LcdLine(1, "No schedules");
		return;
	}
	
	GetScheduleName(CurrentSchedule, str);
	
	sprintf(buf, "~1 %s", str);
	
	LcdLine(1, buf);
}

void ScheduleUp(void)
{
	CurrentSchedule--;
	
	if(CurrentSchedule < 0)
		CurrentSchedule = GetNumSchedules() - 1;
	
	DisplaySchedule();
}

void ScheduleDown(void)
{
	CurrentSchedule++;
	
	if(CurrentSchedule > GetNumSchedules() - 1)
		CurrentSchedule = 0;
	
	DisplaySchedule();
}

void DisplaySelectedSchedule(void)
{
	GetScheduleName(CurrentSchedule, str);
	LcdLine(0, str);
	
	GetScheduleTime(CurrentSchedule, str);
	LcdLine(1, str);
	
	GetScheduleTime(CurrentSchedule, buf);
	sprintf(str, "%s%s", buf, (GetScheduleOn(CurrentSchedule) == 1 ? "   ON" : "  OFF") );
	
	LcdLine(1, str);
}

void ToggleSelectedSchedule(void)
{
	ToggleScheduleOn(CurrentSchedule);
	DisplaySelectedSchedule();
}


/*
	ALARMS
*/
void DisplayAlarm(void)
{
	if(GetNumAlarms() == 0)
	{
		LcdLine(1, "No alarms");
		return;
	}
	
	GetAlarmName(CurrentAlarm, str);
	
	sprintf(buf, "~1 %s", str);
	
	LcdLine(1, buf);
}

void AlarmUp(void)
{
	CurrentAlarm--;
	
	if(CurrentAlarm < 0)
		CurrentAlarm = GetNumAlarms() - 1;
	
	DisplayAlarm();
}

void AlarmDown(void)
{
	CurrentAlarm++;
	
	if(CurrentAlarm > GetNumAlarms() - 1)
		CurrentAlarm = 0;
	
	DisplayAlarm();
}

void DisplaySelectedAlarm(void)
{
	GetAlarmName(CurrentAlarm, str);
	LcdLine(0, str);
	
	GetAlarmTime(CurrentAlarm, buf);
	sprintf(str, "%s%s", buf, (GetAlarmOn(CurrentAlarm) == 1 ? "         ON" : "        OFF") );
	
	LcdLine(1, str);
}

void ToggleSelectedAlarm(void)
{
	ToggleAlarmOn(CurrentAlarm);
	DisplaySelectedAlarm();
}


//Keep backlight on or off
void DisplayLightOFF(void)
{
	SetBacklight(0);
}

void DisplayLightON(void)
{
	SetBacklight(1);
}



//Save all the settings to eeprom when exiting the settings menu
void ExitSettings(void)
{
	SaveSettings();
}

/*
	Alarm Management
*/
void AlarmStartedRinging(void)
{
	navigateToMenu(100);	//Navigate to the alarm menu when an alarm started ringing
}
void AlarmStoppedRinging(void)
{
	navigateToMenu(HOME);	//Navigate back home when an alarm times out
}


void UpdateCurrentAlarm(void)
{
	if(CurrentAlarmDisplay < 3)
		strcpy(str, "[OK] to snooze");
	else
		strcpy(str, "[ESC] to stop");
	
	LcdLine(1, str);
	CurrentAlarmDisplay++;
	
	if(CurrentAlarmDisplay > 5)
		CurrentAlarmDisplay = 0;
}
void DisplayCurrentAlarm(void)
{
	GetCurrentAlarmName(buf);	
	sprintf(str, "~2%s", buf);
	
	LcdLine(0, str);
	
	UpdateCurrentAlarm();
}

/*
	Snooze settings
*/
void DisplaySnooze(void)
{
	sprintf(buf, "~1 %d Min", GetSnooze());
	
	LcdLine(1, buf);
}
void SnoozeUp(void)
{
	int i = GetSnooze();
	
	i++;
	
	if(i >= 30)
		i = 30;
	
	SetSnooze(i);
	DisplaySnooze();
}
void SnoozeDown(void)
{
	int i = GetSnooze();
	
	i--;
	
	if(i <= 1)
		i = 1;
	
	SetSnooze(i);
	DisplaySnooze();
}

/*
	Sync settings
*/
void DisplaySync(void)
{
	if(GetSync() == 0)
		sprintf(buf, "~1 Off");
	else
		sprintf(buf, "~1 %d Min", GetSync());
	
	LcdLine(1, buf);
}
void SyncUp(void)
{
	int i = GetSync();
	
	i++;
	
	if(i >= 60)
		i = 60;
	
	SetSync(i);
	DisplaySync();
}
void SyncDown(void)
{
	int i = GetSync();
	
	i--;
	
	if(i <= 0)
		i = 0;
	
	SetSync(i);
	DisplaySync();
}

const MENU_ITEM_STRUCT menu[] = {

		/*
		 * FIRST TIME SETUP
		 * ITEMS: 1-7
		 */
	// Start setup
	{
		1,	 //ID
		
		&CheckFirstTimeSetup, //onEntry Pointer
		0L, //onActive
		0L, //onExit
		
		0L, //ESC
		0L, //UP
		0L, //DOWN
		0L, //LEFT
		0L, //RIGHT
		0L, //OK
		0L, //ONE
		0L, //TWO
		0L, //THREE
		0L, //FOUR
		0L, //FIVE
		0L, //ALT
		
		//Navigating, ID's of menu on button press
		{
			0, //ESC
			0, //UP
			0, //DOWN
			2  //OK
		},
		
		//Text op het schermpje
		{
			"Start Setup",
			"Press OK"
		}
	},

		// Set Timezone
	{
			2,	 //ID

			&FindTimeZone, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			&ChooseTimeZoneUp, //UP
			&ChooseTimeZoneDown, //DOWN
			0L, //LEFT
			0L, //RIGHT
			&ChooseTimeZone, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					0, //ESC
					0, //UP
					0, //DOWN
					3  //OK
			},

			//Text op het schermpje
			{
					"Timezone",
					"Loading...."
			}
	},

		// Synchronizing
	{
			3,	 //ID

			&SyncTimeEntry, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					0, //ESC
					0, //UP
					0, //DOWN
					0  //OK
			},

			//Text op het schermpje
			{
					"Synchronizing",
					" "
			}
	},

		// Sync Succesfull
	{
			4,	 //ID

			&DisplayDateTime, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					0, //ESC
					0, //UP
					0, //DOWN
					7  //OK
			},

			//Text op het schermpje
			{
					"HH:MM DD/MM/YYYY",
					"Sync succesfull"
			}
	},

		// Sync Failed
	{
			5,	 //ID

			&DisplayDateTime, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					0, //ESC
					0, //UP
					0, //DOWN
					7  //OK
			},

			//Text op het schermpje
			{
					"HH:MM DD/MM/YYYY",
					"Sync Failed"
			}
	},

		// Setup Complete
	{
			7,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			&FirstTimeSetupComplete, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					0, //ESC
					0, //UP
					0, //DOWN
					HOME  //OK
			},

			//Text op het schermpje
			{
					"Setup Complete",
					"Press OK"
			}
	},

		/*
		 * MAIN SCREEN
		 * ITEMS: 8-9
		 */

		// Main Screen (time)
	{
			HOME,	 //ID

			0L, //onEntry Pointer
			&UpdateTimeDisplay, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			&VolumeDown, //LEFT
			&VolumeUp, //RIGHT
			0L, //OK
			&PlayStream1, //ONE
			&PlayStream2, //TWO
			&PlayStream3, //THREE
			&PlayStream4, //FOUR
			&PlayStream5, //FIVE
			&StopStream, //ALT

			//Navigating, ID's of menu on button press
			{
					0, //ESC
					9, //UP
					9, //DOWN
					10  //OK
			},

			//Text op het schermpje
			{
					"HH:MM:SS",
					""
			}
	},
		// Main Screen (date)
	{
			9,	 //ID

			0L, //onEntry Pointer
			&UpdateDateDisplay, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			&VolumeDown, //LEFT
			&VolumeUp, //RIGHT
			0L, //OK
			&PlayStream1, //ONE
			&PlayStream2, //TWO
			&PlayStream3, //THREE
			&PlayStream4, //FOUR
			&PlayStream5, //FIVE
			&StopStream, //ALT

			//Navigating, ID's of menu on button press
			{
					0, //ESC
					8, //UP
					8, //DOWN
					10  //OK
			},

			//Text op het schermpje
			{
					"DD/MM/YYYY",
					""
			}
	},

/*
	 * MAIN MENU
	 * ITEMS: 10-14
	 */
		// Main Menu (Music)
	{
			STREAMING,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					HOME, //ESC
					DEVICE, //UP
					SCHEDULER, //DOWN
					15  //OK
			},

			//Text op het schermpje
			{
					"Menu",
					"~1 1.Streaming"
			}
	},
		// Main Menu (Scheduler)
	{
			SCHEDULER,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					HOME, //ESC
					STREAMING, //UP
					ALARMS, //DOWN
					20  //OK
			},

			//Text op het schermpje
			{
					"Menu",
					"~1 2.Scheduler"
			}
	},

		// Main Menu (Alarm)
	{
			ALARMS,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					HOME, //ESC
					SCHEDULER, //UP
					SETTINGS, //DOWN
					25  //OK
			},

			//Text op het schermpje
			{
					"Menu",
					"~1 3.Alarms"
			}
	},

		// Main Menu (Settings)
	{
			SETTINGS,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					HOME, //ESC
					ALARMS, //UP
					DEVICE, //DOWN
					50  //OK
			},

			//Text op het schermpje
			{
					"Menu",
					"~1 4.Settings"
			}
	},
// Main Menu (Device)
	{
			DEVICE,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					HOME, //ESC
					SETTINGS, //UP
					STREAMING, //DOWN
					80  //OK
			},

			//Text op het schermpje
			{
					"Menu",
					"~1 5.Device"
			}
	},
	/*
	 * MUSIC SOURCE
	 * 15-24
	 */
	{
			15,	 //ID

			&DisplayStream, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			&StreamUp, //UP
			&StreamDown, //DOWN
			0L, //LEFT
			0L, //RIGHT
			&SelectStream, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					STREAMING, //ESC
					0, //UP
					0, //DOWN
					HOME  //OK
			},

			//Text op het schermpje
			{
					"Menu/Streaming",
					"~1 Loading"
			}
	},
	{
			20,	 //ID

			&DisplaySchedule, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			&ScheduleUp, //UP
			&ScheduleDown, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					SCHEDULER, //ESC
					0, //UP
					0, //DOWN
					21  //OK
			},

			//Text op het schermpje
			{
					"Menu/Scheduler",
					"~1 Loading"
			}
	},
	{
			21,	 //ID

			&DisplaySelectedSchedule, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			&ToggleSelectedSchedule, //UP
			&ToggleSelectedSchedule, //DOWN
			&ToggleSelectedSchedule, //LEFT
			&ToggleSelectedSchedule, //RIGHT
			&ToggleSelectedSchedule, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					20, //ESC
					0, //UP
					0, //DOWN
					0  //OK
			},

			//Text op het schermpje
			{
					"Loading",
					""
			}
	},
	{
			25,	 //ID

			&DisplayAlarm, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			&AlarmUp, //UP
			&AlarmDown, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					ALARMS, //ESC
					0, //UP
					0, //DOWN
					26  //OK
			},

			//Text op het schermpje
			{
					"Menu/Alarms",
					"~1 Loading"
			}
	},
	{
			26,	 //ID

			&DisplaySelectedAlarm, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			&ToggleSelectedAlarm, //UP
			&ToggleSelectedAlarm, //DOWN
			&ToggleSelectedAlarm, //LEFT
			&ToggleSelectedAlarm, //RIGHT
			&ToggleSelectedAlarm, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					25, //ESC
					0, //UP
					0, //DOWN
					0  //OK
			},

			//Text op het schermpje
			{
					"Loading",
					""
			}
	},
	/*
	 * SETTINGS
	 * 50-74
	 */
	{
			50,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			&ExitSettings, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					SETTINGS, //ESC
					90, //UP
					51, //DOWN
					55  //OK
			},

			//Text op het schermpje
			{
					"Menu/Settings",
					"~1 1.Sound"
			}
	},
	{
			51,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			&ExitSettings, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					SETTINGS, //ESC
					50, //UP
					52, //DOWN
					61  //OK
			},

			//Text op het schermpje
			{
					"Menu/Settings",
					"~1 2.Display"
			}
	},
	{
			52,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			&ExitSettings, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					SETTINGS, //ESC
					51, //UP
					53, //DOWN
					65  //OK
			},

			//Text op het schermpje
			{
					"Menu/Settings",
					"~1 3.Time/Date"
			}
	},
	{
			53,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			&ExitSettings, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					SETTINGS, //ESC
					52, //UP
					54, //DOWN
					70  //OK
			},

			//Text op het schermpje
			{
					"Menu/Settings",
					"~1 4.Snooze"
			}
	},
	{
			54,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			&ExitSettings, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					SETTINGS, //ESC
					53, //UP
					84, //DOWN
					75  //OK
			},

			//Text op het schermpje
			{
					"Menu/Settings",
					"~1 5.Reset Device"
			}
	},
	/*
	 * SETTINGS / SOUND
	 */
	{
			55,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					50, //ESC
					57, //UP
					56, //DOWN
					58  //OK
			},

			//Text op het schermpje
			{
					"Settings/Sound",
					"~1 1.Volume"
			}
	},
	{
			56,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					50, //ESC
					55, //UP
					57, //DOWN
					59  //OK
			},

			//Text op het schermpje
			{
					"Settings/Sound",
					"~1 2.Bass"
			}
	},
	{
			57,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					50, //ESC
					56, //UP
					55, //DOWN
					60  //OK
			},

			//Text op het schermpje
			{
					"Settings/Sound",
					"~1 3.Treble"
			}
	},
	{
			58,	 //ID

			&VolumeDisplay, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			&VolumeUp, //UP
			&VolumeDown, //DOWN
			&VolumeDown, //LEFT
			&VolumeUp, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					55, //ESC
					0, //UP
					0, //DOWN
					55  //OK
			},

			//Text op het schermpje
			{
					" Volume: ",
					""
			}
	},
	{
			59,	 //ID

			&BassDisplay, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			&BassUp, //UP
			&BassDown, //DOWN
			&BassDown, //LEFT
			&BassUp, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					56, //ESC
					0, //UP
					0, //DOWN
					56  //OK
			},

			//Text op het schermpje
			{
					" Bass: ",
					"",
			}
	},
	{
			60,	 //ID

			&TrebleDisplay, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			&TrebleUp, //UP
			&TrebleDown, //DOWN
			&TrebleDown, //LEFT
			&TrebleUp, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					57, //ESC
					0, //UP
					0, //DOWN
					57  //OK
			},

			//Text op het schermpje
			{
					" Treble:",
					"",
			}
	},
	/*
	 * SETTINGS DISPLAY
	 */
	{
			61,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					51, //ESC
					0, //UP
					0, //DOWN
					62  //OK
			},

			//Text op het schermpje
			{
					"Settings/Display",
					"~1 1.Backlight"
			}
	},
	{
			62,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			&DisplayLightOFF, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					61, //ESC
					63, //UP
					63, //DOWN
					61  //OK
			},

			//Text op het schermpje
			{
					"Light always on",
					"~1 1.NO"
			}
	},
	{
			63,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			&DisplayLightON, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					61, //ESC
					62, //UP
					62, //DOWN
					61  //OK
			},

			//Text op het schermpje
			{
					"Light always on",
					"~1 2.YES"
			}
	},
	/*
	 * SNOOZE TIME
	 */
	{
			65,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					53, //ESC
					0, //UP
					0, //DOWN
					70  //OK
			},

			//Text op het schermpje
			{
					"Settings/Snooze",
					"~1 1.Snooze Time"
			}
	},
	{
			70,	 //ID

			&DisplaySnooze, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			&SnoozeUp, //UP
			&SnoozeDown, //DOWN
			&SnoozeDown, //LEFT
			&SnoozeUp, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					65, //ESC
					0, //UP
					0, //DOWN
					65  //OK
			},

			//Text op het schermpje
			{
					"Snooze/Time",
					"~1 ? Min"
			}
	},
	/*
	 * FACTORY RESET
	 */
	{
			75,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					54, //ESC
					76, //UP
					76, //DOWN
					54  //OK
			},

			//Text op het schermpje
			{
					"Reset device",
					"~1 1.NO"
			}
	},
	{
			76,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					54, //ESC
					75, //UP
					75, //DOWN
					77  //OK
			},

			//Text op het schermpje
			{
					"Reset device",
					"~1 2.YES"
			}
	},
	{
			77,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					54, //ESC
					78, //UP
					78, //DOWN
					54  //OK
			},

			//Text op het schermpje
			{
					"Are you sure?",
					"~1 1.NO"
			}
	},
	{
			78,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			&FactoryResetOk, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					54, //ESC
					77, //UP
					77, //DOWN
					0  //OK
			},

			//Text op het schermpje
			{
					"Are you sure?",
					"~1 2.YES"
			}
	},
	/*
	 * Device
	 */
	{
			80,	 //ID

			&DisplayDeviceID, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					DEVICE, //ESC
					0, //UP
					0, //DOWN
					DEVICE  //OK
			},

			//Text op het schermpje
			{
					"Menu/Device",
					"ID: "
			}
	},
	/*
	 * REBOOT DEVICE
	 */
	{
			84,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			&ExitSettings, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					SETTINGS, //ESC
					54, //UP
					90, //DOWN
					85  //OK
			},

			//Text op het schermpje
			{
					"Menu/Settings",
					"~1 6.Reboot device"
			}
	},
	{
			85,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					84, //ESC
					86, //UP
					86, //DOWN
					84  //OK
			},

			//Text op het schermpje
			{
					"Reboot device",
					"~1 1.NO"
			}
	},
	{
			86,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			&Reboot, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					84, //ESC
					85, //UP
					85, //DOWN
					0  //OK
			},

			//Text op het schermpje
			{
					"Reboot device",
					"~1 2.YES"
			}
	},

	//SYNC SETTINGS

	{
			90,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			&ExitSettings, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					SETTINGS, //ESC
					84, //UP
					50, //DOWN
					91  //OK
			},

			//Text op het schermpje
			{
					"Menu/Settings",
					"~1 7.Web Sync"
			}
	},
	{
			91,	 //ID

			0L, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					90, //ESC
					0, //UP
					0, //DOWN
					92  //OK
			},

			//Text op het schermpje
			{
					"Settings/Sync",
					"~1 1.Sync time"
			}
	},
	{
			92,	 //ID

			&DisplaySync, //onEntry Pointer
			0L, //onActive
			0L, //onExit

			0L, //ESC
			&SyncUp, //UP
			&SyncDown, //DOWN
			&SyncDown, //LEFT
			&SyncUp, //RIGHT
			0L, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					91, //ESC
					0, //UP
					0, //DOWN
					91  //OK
			},

			//Text op het schermpje
			{
					"Sync/Time",
					"~1 ? Min"
			}
	},
	{
			100,	 //ID

			&DisplayCurrentAlarm, //onEntry Pointer
			&UpdateCurrentAlarm, //onActive
			0L, //onExit

			&StopAlarm, //ESC
			0L, //UP
			0L, //DOWN
			0L, //LEFT
			0L, //RIGHT
			&SnoozeAlarm, //OK
			0L, //ONE
			0L, //TWO
			0L, //THREE
			0L, //FOUR
			0L, //FIVE
			0L, //ALT

			//Navigating, ID's of menu on button press
			{
					HOME, //ESC
					0, //UP
					0, //DOWN
					HOME  //OK
			},

			//Text op het schermpje
			{
					"Alarm Ringing",
					""
			}
	}
};

unsigned int curMenuId;
unsigned int keyloop_done;
unsigned int activeCount;

// Let op: confict met display.h ???
typedef enum { LINE1, LINE2 } LCD_LINE_ENUM; 


THREAD(MenuThread, arg)
{
	NutThreadSetPriority(64);  // high prio
	
	handleMenu();
	
	printf("[MENU] End of menu thread\n");
	
	NutThreadExit();
	
	for(;;) {
		NutThreadYield();
	}
}

void InitMenu()	//Start the menu thread
{
	NutThreadCreate("Menu", MenuThread, NULL, 256);
	//handleMenu();
}

//Navigate to a menu item from outsite of the menu
void navigateToMenu(unsigned int menuID)
{
	printf("[MENU] Navigating from %d to %d\n", curMenuId, menuID);
	
	curMenuId = menuID;
	NutSleep(10);
	keyloop_done = 1;
	NutSleep(10);
}

//Main menu handler
void handleMenu(void)
{
	unsigned int idx = 0;		//menu item position in struct array
	unsigned int menuloop_done = 0;	//if menu should end
	unsigned int powerPressed = 0;	//Power button timeout
	keyloop_done = 0;	//if key has been pressed
	unsigned int bl_delay = 0;	//backlight timeout
	curMenuId = 1;	//current menu ID
	activeCount = 0;	//counter to activate the onActive function pointer
	while( !menuloop_done )
	{
		
		/* Clear display */
		LcdClear();
		
		idx = 0;
		/* Lookup current menuID in the static array of menu items */
		while( menu[idx].menuID != curMenuId ) idx++;
		
		/* Write the text to the display */
		LcdLine(0, menu[idx].txt[0]);
		LcdLine(1, menu[idx].txt[1]);
		
		/* Call the onEntry function */
		if( *menu[idx].fp_onEntry )
			(*menu[idx].fp_onEntry)();
		
		/* Loop for key events and act according */
		while( keyloop_done == 0 )
		{
			/* Call the onActive function */
			if(activeCount > 10)
			{
				activeCount = 0;
				
				if( *menu[idx].fp_onActive )
					(*menu[idx].fp_onActive)();
			}
			
			activeCount++;
			
			u_char key = KbGetKey();
		
			switch( key )
			{
				case KEY_ESC:
					if( *menu[idx].fp_onKeyESC )
						(*menu[idx].fp_onKeyESC)();

					
					if(menu[idx].newMenuIdKey[0])
					{
						keyloop_done = 1;
						curMenuId = menu[idx].newMenuIdKey[0];
					}
					
					bl_delay = 0;
					break;

				case KEY_UP:
					if( *menu[idx].fp_onKeyUP )
						(*menu[idx].fp_onKeyUP)();

					if(menu[idx].newMenuIdKey[1])
					{
						keyloop_done = 1;
						curMenuId = menu[idx].newMenuIdKey[1];
					}
					
					bl_delay = 0;
					break;

				case KEY_DOWN:
					if( *menu[idx].fp_onKeyDOWN )
						(*menu[idx].fp_onKeyDOWN)();
					
					if(menu[idx].newMenuIdKey[2])
					{
						keyloop_done = 1;
						curMenuId = menu[idx].newMenuIdKey[2];
					}
					
					bl_delay = 0;
					break;

				case KEY_OK:
					if( *menu[idx].fp_onKeyOK )
						(*menu[idx].fp_onKeyOK)();
					
					if(menu[idx].newMenuIdKey[3])
					{
						keyloop_done = 1;
						curMenuId = menu[idx].newMenuIdKey[3];
					}
					
					bl_delay = 0;
					break;
					
				case KEY_LEFT:
					if( *menu[idx].fp_onKeyLEFT )
						(*menu[idx].fp_onKeyLEFT)();
					
					bl_delay = 0;
					break;
					
				case KEY_RIGHT:
					if( *menu[idx].fp_onKeyRIGHT)
						(*menu[idx].fp_onKeyRIGHT)();
					
					bl_delay = 0;
					break;
					
				case KEY_ALT:
					if( *menu[idx].fp_onKeyALT)
						(*menu[idx].fp_onKeyALT)();
					
					bl_delay = 0;
					break;
					
				case KEY_01:
					if( *menu[idx].fp_onKeyONE)
						(*menu[idx].fp_onKeyONE)();
					
					bl_delay = 0;
					break;
					
				case KEY_02:
					if( *menu[idx].fp_onKeyTWO)
						(*menu[idx].fp_onKeyTWO)();
					
					bl_delay = 0;
					break;
					
				case KEY_03:
					if( *menu[idx].fp_onKeyTHREE)
						(*menu[idx].fp_onKeyTHREE)();
					
					bl_delay = 0;
					break;
					
				case KEY_04:
					if( *menu[idx].fp_onKeyFOUR)
						(*menu[idx].fp_onKeyFOUR)();
					
					bl_delay = 0;
					break;
					
				case KEY_05:
					if( *menu[idx].fp_onKeyFIVE)
						(*menu[idx].fp_onKeyFIVE)();
					
					bl_delay = 0;
					break;
					
				case KEY_POWER:
					//Reboot if the power button is pressed
					powerPressed++;
					if(powerPressed > 5) //0.5 seconds
						Reboot();
					break;
					
				default:
					//No key pressed
					break;
			}
		
			/* Idle loop function */

			/* Backlight switch off */
			if(bl_delay >= BL_SWITCH_OFF_DELAY )
			{
				LcdBackLight(LCD_BACKLIGHT_OFF);
			}
			else
			{
				LcdBackLight(LCD_BACKLIGHT_ON);
				bl_delay++;
			}
			
			if(GetBacklight() == 1)
				bl_delay = 0;
			
			/* Loop delay */
			NutSleep(100);	
		}
		
		powerPressed = 0;
		activeCount = 10;
		keyloop_done = 0;

		/* Keypress is handled, call the menu's exit function. */
		if( *menu[idx].fp_onExit )
			(*menu[idx].fp_onExit)();

		
		NutSleep(250);
	}
}

