#ifndef PLAYER_INC
#define PLAYER_INC

#include <sys/nutconfig.h>
#include <sys/types.h>

//#include <stdlib.h>
//#include <string.h>
#include <stdio.h>
#include <io.h>

//Struct to package arguments to array
typedef struct
{
	FILE* stream;
	TCPSOCKET* sock;
	u_long metaint;
} StreamData;

THREAD(StreamPlayer, arg);

extern int initPlayer(void);
extern char * GetMetaData(void);
extern int play(u_long ip, u_short port, u_char *path);
extern int stop(void);
extern int isStreaming(void);

#endif
