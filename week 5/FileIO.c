/*Dit is de C File om aan te tonen,
hoe je in het niet vluchtig geheugen dingen kan schrijven,
en deze nadat je het kastje heb uitgezet weer op kan halen*/

#include "FileIO.h"

void load_user_setting(void)
{	
	NutNvMemLoad(256, &user_setting, sizeof(user_setting));
	if(user_setting.first_boot != 1)		//check of er al een boot is geweest
	{
		user_setting.first_boot = 1;
		save_user_setting();
	}
	else 
	{
		//doe wat met de data in de struct
	}
}

void save_user_setting(void)
{	
	NutNvMemSave(256, &user_setting, sizeof(user_setting));
}