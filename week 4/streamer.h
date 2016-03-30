#ifndef STREAMER_INC
#define STREAMER_INC

#define MAX_HEADERLINE 512

extern FILE *ConnectStation(TCPSOCKET *sock, u_long ip, u_short port, u_char *path, u_long *metaint);


#endif
