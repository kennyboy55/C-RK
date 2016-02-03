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