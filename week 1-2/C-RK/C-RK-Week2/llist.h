#ifndef LLIST_DEF
#define LLIST_DEF

struct node
{
	int data;
	struct node *next;
	struct node *previous;
};
#endif

void init();
int add(int data, int begin);
int addPos(int data, int positie);
void show(void);
void clear(void);
int length(void);
int exists(int data);
int remove(int);
int removePos(int);

