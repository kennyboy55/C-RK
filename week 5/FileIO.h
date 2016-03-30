#ifndef FileIO_DEF
#define FileIO_DEF

/* User settings */

struct User_Setting
{
	unsigned int first_boot;	
};

/* Loads the user settings struct from the non-volatile memory */
void load_user_setting(void);

/* Saves the user settings struct from the non-volatile memory */
void save_user_setting(void);

struct User_Setting user_setting; //always loads first before using this struct

#endif
