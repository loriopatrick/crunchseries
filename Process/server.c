#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#include "server.h"

struct ServerThreadInfo {
	int sockfd;
	void (*handler)(int sockfd);
	int* active_sockets;
};

void* handleRequest(void* arg) {
	struct ServerThreadInfo* info = (struct ServerThreadInfo*)arg;
	*(info->active_sockets) += 1;
	(info->handler)(info->sockfd);
	*(info->active_sockets) -= 1;
	shutdown(info->sockfd, 2);
	free(arg);
	return 0;
}

void startServer(int port, int max_threads, void (*handler)(int sockfd)) {
	struct sockaddr_in serv_addr;

	int acitve_threads = 0, listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	listen(listenfd, 10);

	for (;;) {
		int connfd = accept(listenfd, 0, 0);
		pthread_t thread;

		struct ServerThreadInfo* info = malloc(sizeof(struct ServerThreadInfo));
		info->sockfd = connfd;
		info->handler = handler;
		info->active_sockets = &acitve_threads;

		while (acitve_threads >= max_threads) sleep(1);
		if (pthread_create(&thread, NULL, handleRequest, (void*)info)) {
			printf("ERROR pthread_create!!\n");
			exit(1);
		}

		pthread_detach(thread);
	}
}