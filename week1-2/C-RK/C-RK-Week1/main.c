#include "main.h"
#include <stdio.h>
#include <string.h>

void main()
{
	/*char string[100];
	int result = strcpy(string, "test");
	printf("result %d, string %s\n", result, string);

	char s1[100];
	strcpy(s1, "remco");

	char s2[100];
	strcpy(s2, "kenneth");

	strCat(&s1, &s2);

	printf("Nieuw string: %s\n", s1);*/

	//gaat goed
	/*
	char s1[10];
	strcpy(s1, "taart");

	char s2[10];
	strcpy(s2, "art");
	
	printf("Result taart en art: %d\n", strend(s1, s2));

	//gaat niet goed, niet hetzelfde
	char s3[10];
	strcpy(s3, "taart");

	char s4[10];
	strcpy(s4, "taa");

	printf("Result taart en taa: %d\n", strend(s3, s4));

	//gaat niet goed, niet s6 > s5
	char s5[10];
	strcpy(s5, "taart");

	char s6[20];
	strcpy(s6, "verjaardagstaart");

	printf("Result taart en verjaardagstaart: %d\n", strend(s5, s6));
	*/

	//fahrenheit();
	//celsius();

	//char str[128];
	//strcpy(str, "Hallo dit is een zin die best wel heel erg lang zou kunnen zijn voor c programming");
	//reverse_string(str);

	//char str[32];
	//strcpy(str, "Test    f    ");
	//printf("%s:\n", str);
	//trim(str);
	//printf("%s:\n", str);

	char str1[32];
	char str2[16];

	strcpy(str1, "acccddddefghijk");
	strcpy(str2, "bdcgij");

	//remove_chars(str1, str2);
	printf("Index of first same char: %d\n",first(str1, str2));
}

void strCat(char* a, char* b)
{
	int i, j;

	i = j = 0;

	while (a[i] != '\0')
		i++;
	while((a[i++] = b[j++]) != '\0')
		;
}

int strend(char s[], char t[])
{
	int sls,slt;

	sls = strlen(s);
	slt = strlen(t);

	if (slt > sls) {
		return 0;
	}
	
	sls--;

	for (int i = slt - 1; i >= 0; i--) 
	{
		if (t[i] != s[sls]) {
			return 0;
		}
		sls--;
	}

	return 1;
}

void fahrenheit(void)
{
	float fahr, celsius;
	int laagste, hoogste, interval;

	laagste = 0;
	hoogste = 300;
	interval = 20;

	fahr = laagste;

	printf("\n F    C\n");
	printf("-----------\n");

	while (fahr <= hoogste)
	{
		celsius = (5.0 / 9.0) * (fahr - 32.0);
		printf("%3.0f %6.1f\n", fahr, celsius);
		fahr = fahr + interval;
	}
}

void celsius(void)
{
	float fahr, celsius;
	int laagste, hoogste, interval;

	laagste = 0;
	hoogste = 150;
	interval = 10;

	celsius = laagste;

	printf("\n C    F\n");
	printf("-----------\n");

	while (celsius <= hoogste)
	{
		fahr = (celsius*1.8) + 32.0;
		printf("%3.0f %6.1f\n", celsius, fahr);
		celsius = celsius + interval;
	}
}

void histogram_length(char * string)
{
	int length = 0;

	strcat(string, " ");

	printf("%s\n", string);

	for (; *string; string++)
	{
		if (*string != ' ' && *string != '\0')
		{
			length++;
		}
		else
		{
			for (int i = 0; i < length; i++)
				printf("-");

			printf("\n");
			length = 0;
		}
	}
}

void histogram_freq(char * string)
{
	int chfreq[128] = {0};

	printf("%s\n", string);

	for (; *string; string++)
	{
		if (*string == ' ')
			continue;

		int chid = *string - '0';
		chfreq[chid]++;
	}

	for (int i = 0; i < 128; i++)
	{
		if (chfreq[i] != 0)
		{
			printf(" %c ", i + '0');

			for (int p = 0; p < chfreq[i]; p++)
				printf("-");

			printf("\n");
		}
	}
}

void reverse_string(char * string)
{
	int chars = strlen(string);

	char *buffer = malloc(sizeof(char) * chars);

	int t = chars - 1;

	for (int i = t; i >= 0; i--)
	{
		buffer[chars - 1 - i] = string[i];
	}

	strncpy(string, buffer, chars);

	printf("%s\n", string);

	free(buffer);
}

void trim(char *str)
{
	char *end;

	if (*str == 0)  // All spaces?
		return;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && isspace(*end)) end--;

	// Write new null terminator
	*(end + 1) = '\0';

	str = end;
}

void remove_chars(char * str1, char * str2)
{
	int chex[128] = { 0 };

	for (; *str2; str2++)
	{
		if (*str2 == ' ')
			continue;

		int chid = *str2 - '0';
		chex[chid] = 1;
	}

	int chars = strlen(str1);
	int count = 0;

	char *buffer = malloc(sizeof(char) * chars);

	for (; *str1; str1++)
	{
		if (chex[*str1 - '0'] != 1)
		{
			printf("Char %c stays\n", *str1);
			buffer[count] = *str1;
			count++;
		}
		else
			printf("Char %c removed\n", *str1);
	}

	str1[count] = '\0';

	strncpy(str1, buffer, count);

	printf("%s\n", str1);

	free(buffer);
}

int first(char * str1, char * str2)
{
	int chex[128] = { 0 };

	for (; *str2; str2++)
	{
		if (*str2 == ' ')
			continue;

		int chid = *str2 - '0';
		chex[chid] = 1;
	}

	int count = 0;

	for (; *str1; str1++)
	{
		if (chex[*str1 - '0'] == 1)
		{			
			return count;
		}
		count++;
	}
	return -1;
}