#ifndef __SERVER_H__
#define __SERVER_H__

void startServer(int port, int max_threads, void (*handler)(int sockfd));
int readNetLen(int sockfd, void* object, int len);

#endif