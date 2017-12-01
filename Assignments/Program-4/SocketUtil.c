#include "SocketUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// For c99 h_addr is h_addr_list[0]
#define h_addr h_addr_list[0]

// Sends the buffer to the socket
// Returns 1 on successful submission
// Returns 0 on failure
int sendData(int socketFd, char* buffer, int bufferSize)
{
    // Sum of chars thats been successfully sent
    int totalCharsWritten = 0;

    // Keep sending data until all of the chars has been sent
    while(totalCharsWritten < bufferSize)
    {
        // Send the buffer and store how many chars thats was sent
        int charsWritten = send(socketFd, &buffer[totalCharsWritten], bufferSize - totalCharsWritten, 0);

        // Error occured if chars sents is < 0
        if(charsWritten < 0)
            return 0;

        // Add the chars sent this time to the sum
        totalCharsWritten += charsWritten;
    }

    // All the chars was sent
    return 1;
}

// Reads socket to the buffer
// Returns 1 on successful read
// Returns 0 on failure
int recvData(int socketFd, char* buffer, int bufferSize)
{
    // Sum of chars read
    int totalCharsRead = 0;

    // Only read chars while it fits within the passed in buffer
    while(totalCharsRead < bufferSize)
    {
        // Read from the socket and store how many chars was read
        int charsRead = recv(socketFd, &buffer[totalCharsRead], bufferSize - totalCharsRead, 0);

        // Error occured if chars read is < 0
        if(charsRead < 0)
            return 0;

        // Add the chars read this time to the sum
        totalCharsRead += charsRead;
    }

    // All the chars were recieved
    return 1;
}

int createClientSocket(int port, struct sockaddr_in* serverAddr)
{
    // Create server address
    memset((char*)serverAddr, '\0', sizeof(*serverAddr));
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(port);

    // Create server host info
    struct hostent* serverHostInfo = gethostbyname("localhost");

    if(serverHostInfo == NULL)
    {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }

    // Copy the address from host to server
    memcpy((char*)&serverAddr->sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

    // Setup socket for connecting to server
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if(socketFd < 0)
    {
        perror("CLIENT: ERROR opening socket");
        exit(1);                                                                                        
    }

    return socketFd;
}

int createServerSocket(int port)
{
    // Create server socket address for connections on the same machine,
    // with the port given in args, and accepts any address for connections
    struct sockaddr_in serverAddr;
    memset((char*)&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Create listen socket
    int listenSocketFd = socket(AF_INET, SOCK_STREAM, 0);

    if(listenSocketFd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    // Enable the socket to begin listening
    if(bind(listenSocketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Error on binding socket");
        exit(1);
    }

    return listenSocketFd;
}

int connectionAccepted(int socketFd, char* clientType)
{
    sendData(socketFd, clientType, 1);

    char buffer[1];
    recvData(socketFd, buffer, sizeof(buffer));

    return buffer[0] == 'Y';
}

int connectionVerified(int socketFd, char acceptedClientType)
{
    char buffer[1];
    recvData(socketFd, buffer, sizeof(buffer));

    if(buffer[0] != acceptedClientType)
        return 0;

    return 1;
}

void sendMessageLength(int socketFd, int length)
{
    char buffer[10];
    sprintf(buffer, "%d", length);
    sendData(socketFd, buffer, sizeof(buffer));
}

int recvMessageLength(int socketFd)
{
    char buffer[10];
    recvData(socketFd, buffer, sizeof(buffer));
    return atoi(buffer);
}

