#ifndef __CRUNCHSERIES_PROCESS_NETWORK_H__
#define __CRUNCHSERIES_PROCESS_NETWORK_H__

int NET_recv(int sockfd, void* object, int len);
int NET_send(int sockfd, void* object, int len);
void NET_startServer(int port, int max_threads, void (*handler)(int sockfd));

#endif