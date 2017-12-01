#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char* argv[])
{
    // Exit if a port was not passed in via args
    if(argc < 2)
    {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);
        exit(1);
    }

    // Get the port number from the args
    int portNum = atoi(argv[1]);

    // Create server socket address for connections on the same machine,
    // with the port given in args, and accepts any address for connections
    struct sockaddr_in serverAddr;
    memset((char*)&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNum);
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

    // Turn on the listen socket to begin listening with a queue of 5
    listen(listenSocketFd, 5);

    // Create client address for storing client info
    struct sockaddr_in clientAddr;
    socklen_t sizeOfClientInfo = sizeof(clientAddr);

    // Accept a connection
    int connectionFd = accept(listenSocketFd, (struct sockaddr*)&clientAddr, &sizeOfClientInfo);

    if(connectionFd < 0)
    {
        perror("Error on accept");
        exit(1);
    }

    // Create buffer to hold msg from client
    char buffer[256];
    memset(buffer, '\0', 256);
    
    // Get msg from client
    int charsRead = recv(connectionFd, buffer, 255, 0);

    if(charsRead < 0)
    {
        perror("Error reading from socket");
        exit(1);
    }

    // Display msg from client
    printf("SERVER: From the client: %s\n", buffer);

    // Send msg to client
    int charsSent = send(connectionFd, "I am the server, I got your msg", 31, 0);

    if(charsSent < 0)
    {
        perror("ERROR writing to socket");
        exit(1);
    }

    // Close connections
    close(connectionFd);
    close(listenSocketFd);
    return 0;
}
