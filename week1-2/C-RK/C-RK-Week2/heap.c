/*Dit is de C file om aan te tonen dat de heap verandert,
wanneer de double linked list wordt aangepast*/

#include "llist.h"
#include <sys/heap.h>
#include "log.h"

void check_heap(void)
{
	add(12,0);//voeg 12 toe aan het einde
	printf("Heapsize: %d\n",NutHeapAvailable());
	
	add(13,0);//voeg 13 toe aan het einde
	printf("Heapsize: %d\n",NutHeapAvailable());
	
	remove(1);//de voorste verwijderen, dus integer 12
	printf("Heapsize: %d\n",NutHeapAvailable());
	
	add(11,1);//voeg 11 toe aan het begin
	printf("Heapsize: %d\n",NutHeapAvailable());
	
	add(14,1);//voeg 14 toe aan het begin
	printf("Heapsize: %d\n",NutHeapAvailable());
	
	add(15,0);//voeg 15 toe aan het begin
	printf("Heapsize: %d\n",NutHeapAvailable());
	
	removePos(2);//verwijder positie 1, dus integer 11
	printf("Heapsize: %d\n",NutHeapAvailable());
	
}