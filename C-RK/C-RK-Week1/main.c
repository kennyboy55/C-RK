#include "main.h"
#include <stdio.h>
#include <string.h>

void main()
{
	char string[100];
	int result = strcpy(string, "test");
	printf("result %d, string %s\n", result, string);

	char s1[100];
	strcpy(s1, "remco");

	char s2[100];
	strcpy(s2, "kenneth");

	strCat(&s1, &s2);

	printf("Nieuw string: %s\n", s1);
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