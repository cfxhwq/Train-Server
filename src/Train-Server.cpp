//============================================================================
// Name        : Train-Server.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <iostream>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

#define PORT 1234
#define MAXDATASIZE 1000
#define BACKLOG 2

void process_cli(int connectfd, struct sockaddr_in client);

int main() {

	int listenfd, connectfd;
	pid_t pid;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t addrlen;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket() error.");
		exit(-1);
	}

	int opt = SO_REUSEADDR;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))
			== -1) {
		perror("setsockopt() error ");
		exit(-1);
	}

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenfd, (struct sockaddr *) &server, sizeof(server)) == -1) {
		cout << server.sin_addr.s_addr << endl;
		perror("bind() error");
		exit(-1);
	}
	cout << "bind finish" << endl;

	if (listen(listenfd, BACKLOG) == -1) {
		perror("listen() error");
		exit(-1);
	}
	cout << "listen finish" << endl;

	addrlen = sizeof(client);
	while (1) {
		if ((connectfd = accept(listenfd, (struct sockaddr *) &client, &addrlen))
				== -1) {
			perror("accept() error");
			exit(-1);
		}
		if ((pid = fork()) > 0) {
			cout << "father" << endl;
			close(connectfd);
			continue;
		} else if (pid == 0) {
			cout << "son" << endl;
			close(listenfd);
			process_cli(connectfd, client);
			exit(1);
		} else {
			printf("fork() error\n");
			exit(0);
		}
	}
	close(listenfd);
}

void process_cli(int connectfd, struct sockaddr_in client) {
	int num;
	char recvbuf[MAXDATASIZE], sendbuf[MAXDATASIZE];
	printf("process from %s:%d\n", inet_ntoa(client.sin_addr),
			ntohs(client.sin_port));
	send(connectfd, "Welcome!\n", 9, 0);

	while (num = recv(connectfd, recvbuf, MAXDATASIZE, 0)) {
		recvbuf[num] = '\0';
		printf("Msg: %s,len:%d\n", recvbuf,num);
		int i = 0;
		for (i = 0; i < num; ++i) {
			sendbuf[i] = recvbuf[num - i - 1];
		}
		sendbuf[num] = '\0';
		send(connectfd, sendbuf, strlen(sendbuf), 0);
	}
	close(connectfd);
}
