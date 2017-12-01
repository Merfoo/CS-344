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

int main(int argc, char* argv[])
{
    // Exit if port wasn't passed in via args
    if(argc < 2)
    {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);
        exit(0);
    }

    // Get port number from args
    int portNum = atoi(argv[1]);

    // Create server address
    struct sockaddr_in serverAddr;
    memset((char*)&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNum);
    
    // Create server host info
    struct hostent* serverHostInfo = gethostbyname("localhost");

    if(serverHostInfo == NULL)
    {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    } 

    // Copy the address from host to server
    memcpy((char*)&serverAddr.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);
    
    // Setup socket for connecting to server
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if(socketFd < 0)
    {
        perror("CLIENT: ERROR opening socket");
        exit(1);
    }

    // Connect to server
    if(connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("CLIENT: ERROR connection");
        exit(1);
    }

    // Get input msg from user
    printf("CLIENT: Enter text to send to server: ");    
    char buffer[256];
    memset(buffer, '\0', sizeof(buffer));
    fgets(buffer, sizeof(buffer) - 1, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    // Send msg to server
    int charsWritten = send(socketFd, buffer, strlen(buffer), 0);

    if(charsWritten < 0)
    {
        perror("CLIENT: ERROR writing to socket");
        exit(1);
    }

    if(charsWritten < strlen(buffer))
        printf("CLIENT: WARNING: Not all data written to socket!\n");

    // Get return msg from server
    memset(buffer, '\0', sizeof(buffer));
    int charsRead = recv(socketFd, buffer, sizeof(buffer) - 1, 0);

    if(charsRead < 0)
    {
        perror("CLIENT: ERROR reading from socket");
        exit(1);
    }

    // Display msg from server
    printf("CLIENT: I received this message from server: \"%s\"\n", buffer);

    // Close socket
    close(socketFd);
    return 0;
}
