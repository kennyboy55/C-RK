#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <io.h>

#include <dev/debug.h>
#include <dev/nicrtl.h>
#include <dev/vs1001k.h>

#include <sys/version.h>
#include <sys/confnet.h>
#include <sys/heap.h>
#include <sys/bankmem.h>
#include <sys/thread.h>
#include <sys/timer.h>

#include <dev/board.h>
#include <pro/httpd.h>
#include <pro/dhcp.h>
#include <pro/asp.h>
#include <pro/discover.h>
#include <dev/nicrtl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include <pro/dhcp.h>

#include "streamer.h"
#include "player.h"
#include "web.h"

#define OK 1
#define NOK 0
/*!
 * \brief Connect to a radio station.
 *
 * \param sock TCP socket for this connection.
 * \param ip   IP address of the server to connect.
 * \param port Port number of the server to connect.
 *
 * \return Stream pointer of the established connection on success.
 *         Otherwise 0 is returned.
 */
FILE *ConnectStation(TCPSOCKET *sock, u_long ip, u_short port, u_char *path, u_long *metaint){
    FILE *stream;
    u_char *line;
    u_char *cp;
	
	stream = getStream(sock, ip,port);

    /*
     * Send the HTTP request.
     */
    printf("[STREAMER] GET %s HTTP/1.0\n\n", path);
    fprintf(stream, "GET %s HTTP/1.0\r\n", path);
    fprintf(stream, "Host: %s\r\n", inet_ntoa(ip));
    fprintf(stream, "User-Agent: Ethernut\r\n");
    fprintf(stream, "Accept: */*\r\n");
    fprintf(stream, "Icy-MetaData: 1\r\n");
    fprintf(stream, "Connection: close\r\n");
    fputs("\r\n", stream);
    fflush(stream);

    /*
     * Receive the HTTP header.
     */
    line = malloc(MAX_HEADERLINE);
    while(fgets((char*) line, MAX_HEADERLINE, stream)) {

        /*
         * Chop off the carriage return at the end of the line. If none
         * was found, then this line was probably too large for our buffer.
         */
        cp = (u_char *) strchr((char*) line, '\r');
        if(cp == 0) {
            printf("[STREAMER] Warning: Input buffer overflow\n");
            continue;
        }
        *cp = 0;

        /*
         * The header is terminated by an empty line.
         */
        if(*line == 0) {
            break;
        }
        if(strncmp((char*) line, "icy-metaint:", 12) == 0) {
            *metaint = atol( (char*) (line + 12));
        }
        printf("[STREAMER] line of metaint: %s\n", line);
    }
    putchar('\n');

    free(line);

    return stream;
}
