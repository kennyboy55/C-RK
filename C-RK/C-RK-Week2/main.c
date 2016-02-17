#include <stdio.h>
#include "llist.h"

int main()
{
	int idx;	

	printf("Running init...\n");

	init();

	printf("Adding data...\n");

	for(idx = 0; idx < 10; idx++)
	{
		add(idx);
	}		

	printf("Showing data...\n");
	show();

	printf("Clearing data...\n");
	clear();

	printf("Showing data...\n");
	show();

	printf("Done\n");

	return 1;
}
