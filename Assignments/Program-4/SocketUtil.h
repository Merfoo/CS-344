#pragma once

#include <netinet/in.h>

int sendData(int socketFd, char* buffer, int bufferSize);

int recvData(int socketFd, char* buffer, int bufferSize);

int createClientSocket(int port, struct sockaddr_in* serverAddr);

int createServerSocket(int port);

int connectionAccepted(int socketFd, char* clientType);

int connectionVerified(int socketFd, char acceptedClientType);

void sendMessageLength(int socketFd, int length);

int recvMessageLength(int socketFd);

