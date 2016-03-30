#ifndef MENU_INCLUDE
#define MENU_INCLUDE

/* Defines */
#define BL_SWITCH_OFF_DELAY 100	// lcd back-light switches of 10 sec after idle
#define MAX_MENU_KEY 	4
#define LCD_MAX_LINES	2
/* Exported functions */
void handleMenu(void);
void InitMenu(void);
void navigateToMenu(unsigned int);

void AlarmStartedRinging(void);
void AlarmStoppedRinging(void);

typedef struct
{
	unsigned int menuID;						/* (Uniek) MenuID			*/
	
	//Navigating
	void (*fp_onEntry)(void);					/* On entry function call	*/
	void (*fp_onActive)(void);					//On active function call
	void (*fp_onExit)(void);					/* On exit function call	*/

	//Key events
	void (*fp_onKeyESC)(void);					/* function call for key's  */
	void (*fp_onKeyUP)(void);	
	void (*fp_onKeyDOWN)(void);
	void (*fp_onKeyLEFT)(void);
	void (*fp_onKeyRIGHT)(void);
	void (*fp_onKeyOK)(void);
	void (*fp_onKeyONE)(void);
	void (*fp_onKeyTWO)(void);
	void (*fp_onKeyTHREE)(void);
	void (*fp_onKeyFOUR)(void);
	void (*fp_onKeyFIVE)(void);
	void (*fp_onKeyALT)(void);

	//Quick navigating
	unsigned int newMenuIdKey[MAX_MENU_KEY];	/* New menuId when key pressed */ 
	
	//Text and other data
	char *txt[LCD_MAX_LINES];						/* LCD_MAX_LINES lines of LCD text (or NULL) */

} MENU_ITEM_STRUCT;

#endif

