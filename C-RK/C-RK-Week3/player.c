
#define LOG_MODULE  LOG_PLAYER_MODULE

#include <stdio.h>
#include <string.h>

#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/version.h>
#include <dev/irqreg.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <pro/dhcp.h>
#include <pro/sntp.h>

#include <sys/confnet.h>

#include <dev/board.h>
#include <pro/httpd.h>
#include <pro/asp.h>
#include <pro/discover.h>
#include <dev/nicrtl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include "player.h"
#include "vs10xx.h"
#include "streamer.h"
#include "settings.h"
#include <sys/bankmem.h>

#define OK			1
#define NOK		0

int stopped = 1;
int stopstream = 0;

char* MetaData;

int initPlayer(void)
{
	MetaData = (char *) calloc(128, sizeof(char));
	
	SetBass(GetBass());
	SetTreble(GetTreble());
	SetVolume(GetVolume());
	return OK;
}

char* GetMetaData(void)
{
	return MetaData;
}

int isStreaming()
{
	return !stopped;
}

int ProcessMetaData(FILE *stream)
{
    u_char blks = 0;
    u_short cnt;
    int got;
    int rc = 0;
    u_char *mbuf;

    /*
     * Wait for the lenght byte.
     */
    got = fread(&blks, 1, 1, stream);
    if(got != 1) {
        return -1;
    }
    if (blks) {
        if (blks > 32) {
            printf("[PLAYER] Error: Metadata too large, %u blocks\n", blks);
            return -1;
        }

        cnt = blks * 16;
        if ((mbuf = malloc(cnt + 1)) == 0) {
            return -1;
        }

        /*
         * Receive the metadata block.
         */
        for (;;) {
            if ((got = fread(mbuf + rc, 1, cnt, stream)) <= 0) {
                return -1;
            }
            if ((cnt -= got) == 0) {
                break;
            }
            rc += got;
            mbuf[rc] = 0;
        }
		
		memset(&MetaData[0], 0, sizeof(MetaData));
		
		int count = 0;
		int copy = 0;
		
		int startval = 0;
		int stopval = 0;
		
		for(;;)
		{
			
			if(mbuf[count] == '\'' && copy == 1)
			{
				copy = 0;
				stopval = count-1;
				break;
			}
			
			if(mbuf[count] == '\'' && copy == 0)
			{
				startval = count+1;
				copy = 1;
			}
			
			count++;
		}
		
		int len = stopval - startval;
		len = len > 127 ? 127 : len;
		
		strncpy(MetaData, (char*) &mbuf[startval], len);
		
        free(mbuf);
    }
    return 0;
}

int stop(void)
{
	if(stopped == 1)
		return 0;
	
	printf("[PLAYER] Stopping other stream\n");
	
	stopstream = 1;
	
	while(!stopped) { NutSleep(5); };
	printf("[PLAYER] Stream stopped\n");
	
	//Give thread time to clean up
	NutSleep(100);
	
	strcpy(MetaData, "");
	
	return 1;
}

int play(u_long ip, u_short port, u_char *path)
{
	stop();	//Stop any old threads
	
	FILE *stream;
	TCPSOCKET *sock = 0;
	u_long metaint;
	
	stream = ConnectStation(sock, ip, port, path, &metaint);	//Connect to a stream
	
	if(!stream)
	{
		printf("[PLAYER] Not stream\n");
		return NOK;
	}
	
	//Package 3 variables in a struct to pass them to the thread.
	StreamData *arguments = malloc(sizeof(StreamData));
    arguments->stream = stream;
	arguments->sock = sock;
    arguments->metaint = metaint;
	
	//Start thread with packaged arguments
	NutThreadCreate("Streamer", StreamPlayer, arguments, 512);
	
	//Await thread start
	NutSleep(20);
	
	//Clear the packege
	free(arguments);
	
	return OK;
}

THREAD(StreamPlayer, arg)
{
	
	NutThreadSetPriority(1);  // high prio

	printf("[PLAYER] Play thread created. Device is playing stream now !\n");
	
	StreamData* f = (StreamData *) arg;
	
	FILE* stream = f->stream;
	TCPSOCKET* sock = f->sock;
	u_long metaint = f->metaint;
	
	size_t rbytes;
    u_char *mp3buf;
    u_char ief;
    int got = 0;
    u_long last;
    u_long mp3left = metaint;
	int endstream = 0;

    /*
     * Initialize the MP3 buffer. The NutSegBuf routines provide a global
     * system buffer, which works with banked and non-banked systems.
     */
    if (NutSegBufInit(8192) == 0) {
        printf("[PLAYER] Error: MP3 buffer init failed\n");
        NutThreadExit();
    }

    /*
     * Initialize the MP3 decoder hardware.
     */
    if (VsPlayerReset(0)) {
        printf("[PLAYER] Error: MP3 hardware init failed\n");
        NutThreadExit();
    }
	
	stopped = 0;

    /*
     * Reset the MP3 buffer.
     */
    ief = VsPlayerInterrupts(0);
    NutSegBufReset();
    VsPlayerInterrupts(ief);
    last = NutGetSeconds();

    for (;;) {
		
		if(stopstream)
		{
			break;
		}
	
        /*
         * Query number of byte available in MP3 buffer.
         */
        ief = VsPlayerInterrupts(0);
        mp3buf = (u_char*) NutSegBufWriteRequest(&rbytes);
        VsPlayerInterrupts(ief);

        /*
         * If the player is not running, kick it.
         */
        if (VsGetStatus() != VS_STATUS_RUNNING) {
            printf("[PLAYER] Not running\n");
            if(rbytes < 1024 || NutGetSeconds() - last > 4UL) {
                last = NutGetSeconds();
                printf("[PLAYER] Kick player\n");
                VsPlayerKick();
            }
        }
        /*
         * Do not read pass metadata.
         */
        if (metaint && rbytes > mp3left) {
            rbytes = mp3left;
        }

        /*
         * Read data directly into the MP3 buffer.
         */
        while (rbytes) {
			
            if ((got = fread(mp3buf, 1, rbytes, stream)) > 0) {
                ief = VsPlayerInterrupts(0);
                mp3buf = (u_char *) NutSegBufWriteCommit(got);
                VsPlayerInterrupts(ief);

                if (metaint) {
                    mp3left -= got;
                    if (mp3left == 0) {
                        ProcessMetaData(stream);
                        mp3left = metaint;
                    }
                }

                if(got < rbytes && got < 512) {
                    //printf("%lu buffered\n", NutSegBufUsed());
                    NutSleep(250);
                }
                else {
                    NutThreadYield();
                }
            } else {
				printf("[PLAYER] Failed to read from stream, closing\n");
                break;
            }
            rbytes -= got;
        }

        if(got <= 0) {
            endstream++;
			
			printf("[PLAYER] Stream ending, %d tries left\n", 3 - endstream);
			
			if(endstream >= 3)
			{
				printf("[PLAYER] End of stream, closing\n");
				break;
			}
        }
		else
			endstream = 0;
    }
	
	VsPlayerStop(); //Stop the audio
	
    while (VsGetStatus() == VS_STATUS_RUNNING) {
        NutSleep(50);	//Wait until player has really stopped
    }
	
	fclose(stream);	//Close stream
	NutTcpCloseSocket(sock);	//Close socket

	strcpy(MetaData, "");	//Clear meta data
	
	stopstream = 0;	//Clear stop flag
	stopped = 1;	//Mark thread as stopped
	
	printf("[PLAYER] End of play thread\n");

	NutThreadExit();	//Clean up thread and exit
	
	for(;;)
	{
		printf("[PLAYER] PLAY THREAD NOT STOPPED\n");
		NutThreadYield();
	}
}



