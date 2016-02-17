#include "stdlib.h"
#include "stdio.h"
#include "llist.h"


static struct node *pEnd = NULL;
static struct node *pStart = NULL;

void init()
{
	pEnd = NULL;
	pStart = NULL;
}

/*
 * Aan de voorkant invoegen
 */
int add(int data, int begin)
{
	struct node *pn = (struct node*)malloc(sizeof(struct node));

	if(NULL ==  pn)
	{
		printf("Out of memory ...\n");
	}
	else
	{
		if (NULL == pStart)
		{
			pn->data = data;
			pn->previous = NULL;
			pn->next = NULL;
			pStart = pn;
			pEnd = pn;
		}
		else if (begin)
		{
			pStart->previous = pn;
			pn->data = data;
			pn->next = pStart;
			pn->previous = NULL;
			pStart = pn;
		}
		else
		{
			pEnd->next = pn;
			pn->data = data;
			pn->previous = pEnd;
			pn->next = NULL;
			pEnd = pn;
		}
	}

	return 1;	
}

int addPos(int data, int positie)
{
	struct node *pn = (struct node*)malloc(sizeof(struct node));

	if (NULL == pn)
	{
		printf("Out of memory ...\n");
	}
	else
	{
		if (NULL == pStart)
		{
			add(data, '1');
		}
		else
		{
			if (positie < 0)
			{
				printf("Positie < 0 : %d ...\n", positie);
				return 0;
			}

			int idx = 0;
			struct node *p = pStart;

			for (; NULL != p->next; p = p->next)
			{
				if (positie == idx)
				{
					pn->data = data;
					pn->next = p;
					pn->previous = p->previous;
					p->previous->next = pn;
					p->previous = pn;
					return 1;
				}

				idx++;
			}

			printf("Positie > %d : %d ...\n", idx, positie);
			free(pn);
			return 0;
		}
	}

	return 1;
}

void show()
{
	struct node *p = pStart;
	int nr = 0;
	
	if (NULL == pStart)
	{
		printf("De lijst is leeg\n");
	}
	else
	{
		for (; NULL != p->next; p = p->next)
		{
			printf("node nr: %d heeft data [%d]\n", nr++, p->data);
		}
	}
}

void clear()
{
	struct node *p = pEnd;
	struct node *prev = NULL;

	for (; NULL != p->previous;)
	{
		prev = p->previous;
		free(p);
		p = prev;
	}

	pEnd = NULL;
	pStart = NULL;

	printf("De gehele lijst in gecleared!\n");
}

int length()
{
	int idx = 0;
	struct node *p = pStart;

	for (; NULL != p->next; p = p->next)
	{		
		idx++;
	}

	return idx;
}

int exists(int data)
{
	int idx = 0;
	struct node *p = pStart;

	for (; NULL != p->next; p = p->next)
	{
		if (p->data == data)
		{
			return idx;
		}

		idx++;
	}

	return -1;
}


