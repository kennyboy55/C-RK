#include <stdio.h>
#include "llist.h"

int main()
{
	int idx;	

	printf("Running init...\n");

	init();

	printf("Adding data at end...\n");

	for(idx = 0; idx < 10; idx++)
	{
		add(idx, '0');
	}		

	printf("Showing data...\n");
	show();

	printf("Clearing data...\n");
	clear();

	printf("Showing data...\n");
	show();

	printf("Adding data at begin and end...\n");

	for (idx = 0; idx < 10; idx++)
	{
		if (idx % 2 == 0)
			add(idx, 1);
		else
			add(idx, 0);
	}

	addPos(22, 5);
	addPos(22, 40);
	addPos(22, -10);

	printf("Showing data...\n");
	show();

	printf("Length of list: %d\n", length());
	printf("Position of 22 in list: %d\n", exists(22));
	printf("Position of 40 in list: %d\n", exists(40));

	printf("Clearing data...\n");
	clear();

	printf("Done\n");

	return 1;
}
