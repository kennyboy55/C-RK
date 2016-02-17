#include "stdlib.h"
#include "stdio.h"
#include "llist.h"


static struct node *pHead = NULL;

void init()
{
	pHead = NULL;

}

/*
 * Aan de voorkant invoegen
 */
int add(int data)
{
	struct node *pn = (struct node*)malloc(sizeof(struct node));

	if(NULL ==  pn)
	{
		printf("Out of memory ...");
	}
	else
	{
		if(NULL == pHead)
		{
			pn->data = data;
			pn->next = NULL;
			pHead = pn;
		}
		else
		{
			pn->data = data;
			pn->next = pHead;
			pHead = pn;
		}
	}

	return 1;	
}

void show()
{
	struct node *p = pHead;
	int nr = 0;
	
	for( ; NULL !=  p->next ; p = p->next )
	{
		printf("node nr: %d heeft data [%d]\n",nr++,p->data);
	}
}


