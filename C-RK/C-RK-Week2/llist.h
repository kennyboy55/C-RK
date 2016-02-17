#ifndef LLIST_DEF
#define LLIST_DEF

struct node
{
	int data;
	struct node *next;
	struct node *previous;
};

void init();
int add(int data);
void show();

#endif
