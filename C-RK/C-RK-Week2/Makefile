# Voorbeeld Makefile
#
# @ Diederich Kroeske, 2011
#

CC	= 	gcc
#CC	= 	avr-gcc
	
TARGET	=	llexample	
CFLAGS	= 	-Wall -O2

all	:	$(TARGET)

llist.o	:	llist.c llist.h
		echo $@
		$(CC) -c llist.c $(CFLAGS)

main.o	:	main.c
		$(CC) -c main.c $(CFLAGS)

$(TARGET):	main.o llist.o
		$(CC) main.o llist.o $(CFLAGS) -o $(TARGET)

clean	:
		rm -f $(TARGET) main.o llist.o

info	:
		@echo "$(CC) using $(CFLAGS)"
