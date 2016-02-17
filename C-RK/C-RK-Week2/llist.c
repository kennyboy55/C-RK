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
		printf("Out of memory ...\n");
	}
	else
	{
		if(NULL == pHead)
		{
			pn->data = data;
			pn->previous = NULL;
			pn->next = NULL;
			pHead = pn;
		}
		else
		{
			pHead->next = pn;
			pn->data = data;
			pn->previous = pHead;
			pn->next = NULL;
			pHead = pn;
		}
	}

	return 1;	
}

void show()
{
	struct node *p = pHead;
	int nr = 0;
	
	if (NULL == pHead)
	{
		printf("De lijst is leeg\n");
	}
	else
	{
		for (; NULL != p->previous; p = p->previous)
		{
			printf("node nr: %d heeft data [%d]\n", nr++, p->data);
		}
	}
}

void clear()
{
	struct node *p = pHead;
	struct node *prev = NULL;

	for (; NULL != p->previous;)
	{
		prev = p->previous;
		free(p);
		p = prev;
	}

	pHead = NULL;

	printf("De gehele lijst in gecleared!\n");
}


