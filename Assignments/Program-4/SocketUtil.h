#pragma once

#include <netinet/in.h>

// Sends the buffer to the socket
// Returns 1 on successful submission
// Returns 0 on failure
int sendData(int socketFd, char* buffer, int bufferSize);

// Sends the buffer to the socket
// Returns 1 on successful submission
// Reads socket to the buffer
// Returns 1 on successful read
// Returns 0 on failure
int recvData(int socketFd, char* buffer, int bufferSize);

// Creates a client socket on the port and fills in the serverAddr thats passed in
// Returns the file descriptor for the newly created socket
int createClientSocket(int port, struct sockaddr_in* serverAddr);

// Creates a server socket on the passed in port
// Returns the file descriptor of the newly created socket
int createServerSocket(int port);

// Sends a message about the type of client it is and receives a message if 
// the connection got accepted
// Returns 1 if accepted
// Returns 0 otherwise
int connectionAccepted(int socketFd, char* clientType);

// Receives message about the client type, comparing it against the
// passed in accepted client. 
// Returns 1 if they match
// Returns 0 otherwise
int connectionVerified(int socketFd, char acceptedClientType);

// Sends a message about the length of incoming data
void sendMessageLength(int socketFd, int length);

// Receives a message about the length of incoming data
int recvMessageLength(int socketFd);

