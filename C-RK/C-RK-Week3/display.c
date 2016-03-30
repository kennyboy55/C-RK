
char customCharArray[64] = {
 0b00000,0b01000,0b11111,0b01000,0b00010,0b11111,0b00010,0b00000, //Web sync true
 0b00000,0b00100,0b01110,0b00000,0b00000,0b01110,0b00100,0b00000, //Up Down
 0b00100,0b01110,0b01110,0b01110,0b11111,0b00000,0b00100,0b00000, //Alarm Clock
 0b00000,0b00100,0b01101,0b11101,0b01101,0b00100,0b00000,0b00000, //Volume ON
 0b00000,0b00100,0b01100,0b11100,0b01100,0b00100,0b00000,0b00000, //Volume OFF
 0b00000,0b00001,0b00011,0b00111,0b01111,0b11111,0b00000,0b00000, //Network TRUE
 0b00000,0b01000,0b01100,0b01110,0b01100,0b01000,0b00000,0b00000, //PLAY
 0b00000,0b01110,0b10101,0b10111,0b10001,0b01110,0b00000,0b00000  //Time Sync TRUE
};


void LcdClear()
{
    LcdWriteByte(WRITE_COMMAND, 0x01);          // display clear
    NutDelay(5);
	LcdWriteByte(WRITE_COMMAND, 0x80);			// DD-RAM address counter (cursor pos) to '0'
}

void LcdLine(u_char line, char text[])
{
	int i;
	
	if(line == 0)	//Top line
	{
		LcdWriteByte(WRITE_COMMAND, 0x80); // command to write to line 1	
		for(i = 0; i< 16; i++)
		{
			LcdChar(32);	//Clear the line
		}
		LcdWriteByte(WRITE_COMMAND, 0x80); // command to write to line 1
	}
	else if(line == 1)	//Bottom line
	{
		LcdWriteByte(WRITE_COMMAND, 0xC0); // command to write to line 2
		for(i = 0; i< 16; i++)
		{
			LcdChar(32);	//Clear the line
		}
		LcdWriteByte(WRITE_COMMAND, 0xC0); // command to write to line 1
	}
	else
	{
		return;
	}
	
	int count = 0;
	
	while(text[count] != '\0')
	{
		//Replace ~n with the decimal n
		//This displays the custom character
		if(text[count] == '~')
		{
			count++;
			char num = text[count];
			
			int number = num - '0';
			
			LcdWriteByte(WRITE_DATA, number);
		}
		else
		{
			//Write char to display
			LcdChar(text[count]);
		}
		
		count++;
		
		if(count > 24)
			break;
		
		NutSleep(1);
	}
}

void LcdScrollLine(u_char line, int pos, char text[])
{
	int i;
	
	if(line == 0)
	{
		LcdWriteByte(WRITE_COMMAND, 0x80); // command to write to line 1	
		for(i = 0; i< 16; i++)
		{
			LcdChar(32);
		}
		LcdWriteByte(WRITE_COMMAND, 0x80); // command to write to line 1
	}
	else if(line == 1)
	{
		LcdWriteByte(WRITE_COMMAND, 0xC0); // command to write to line 2
		for(i = 0; i< 16; i++)
		{
			LcdChar(32);
		}
		LcdWriteByte(WRITE_COMMAND, 0xC0); // command to write to line 1
	}
	else
	{
		return;
	}
	
	//Start at scroll location
	int count = pos;
	
	while(text[count] != '\0')
	{
		if(text[count] == '~')
		{
			count++;
			char num = text[count];
			
			int number = num - '0';
			
			LcdWriteByte(WRITE_DATA, number);
		}
		else
		{
			LcdChar(text[count]);
		}
		
		count++;
		
		if(count > 24 + pos)
			break;
		
		NutSleep(1);
	}
}

//Write data to the Custom Character RAM
void LcdCGRAM(char *icon, int length)
{
	//SELECT CGRAM
	LcdWriteByte(WRITE_COMMAND, 0x40);
	
	int count;
	
	//Write the array with custom icons
	for(count = 0; count < length; count++)
	{
		LcdChar(icon[count]);
		NutSleep(1);
	}
	
	//Select the display RAM
	LcdWriteByte(WRITE_COMMAND, 0x80);
}

void LcdSetPosition(int line)
{
	if(line)
	{
		LcdWriteByte(WRITE_COMMAND, 0xC0);
	}
	else
	{
		LcdWriteByte(WRITE_COMMAND, 0x80);
	}
}
