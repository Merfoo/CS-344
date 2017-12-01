#include "EncryptServer.h"
#include "SocketUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

#define MAX_CONCURRENT_CONNECTIONS 5

char* encryptText(char* text, int textLength, char* key)
{
    char* encText = malloc((textLength + 1) * sizeof(char));

    for(int i = 0; i < textLength; i++)
    {
        char textChar = text[i] - 'A';
        char keyChar = key[i] - 'A';

        if(text[i] == ' ')
            textChar = 26;

        if(key[i] == ' ')
            keyChar = 26;

        encText[i] = ((textChar + keyChar) % 27) + 'A';
    }

    encText[textLength] = '\0';
    return encText;
}

char* decryptText(char* text, int textLength, char* key)
{
    char* decText = malloc((textLength + 1) * sizeof(char));

    for(int i = 0; i < textLength; i++)
    {
        char textChar = text[i] - 'A';
        char keyChar = key[i] - 'A';

        if(text[i] == ' ')
            textChar = 26;

        if(key[i] == ' ')
            keyChar = 26;
        
        decText[i] = ((textChar - keyChar));
        
        if(decText[i] < 0)
            decText[i] += 27;

        if(decText[i] == 26)
            decText[i] = ' ';

        else
            decText[i] += 'A';
    }

    decText[textLength] = '\0';
    return decText;
}

int acceptConnection(int connectionFd)
{ 
    pid_t newPid = fork();

    // If pid from fork is 0, we're the child fork
    if(newPid == 0)
    {
        // Send message that the connection has been accepted
        sendData(connectionFd, "Y", 1);

        // Receive length of plaintext and get plaintext
        int textLength = recvMessageLength(connectionFd);
        char* text = malloc(textLength * sizeof(char));
        recvData(connectionFd, text, textLength);

        // Receive length of key and get key
        int keyLength = recvMessageLength(connectionFd);
        char* key = malloc(keyLength * sizeof(char));
        recvData(connectionFd, key, keyLength);
        key[keyLength] = '\0';

        printf("Plaintext: %d %s\n", textLength, text);
        printf("Key: %d %s\n", keyLength, key);
        
        char* enc = encryptText(text, textLength, key);
        char* dec = decryptText(enc, textLength, key);

        printf("Enc: %s\nDec: %s\n", enc, dec);
        exit(0);
    }

    // Return child pid
    return newPid;
}

int main(int argc, char* argv[])
{
    // Exit if a port was not passed in via args
    if(argc < 2)
    {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);
        exit(1);
    }

    // Get the port number from the args
    int port = atoi(argv[1]);

    // Turn on the listen socket to begin listening with a queue of 5
    int listenSocketFd = createServerSocket(port);
    listen(listenSocketFd, 5);

    // Create client address for storing client info
    struct sockaddr_in clientAddr;
    socklen_t sizeOfClientInfo = sizeof(clientAddr);

    // Variables to keep track of the all the connections
    int connections[MAX_CONCURRENT_CONNECTIONS] = {0};
    int activeConnections = 0;

    // Keep accepting connections by forking new child processes
    while(1)
    {
        // Loop through all connections, checking if they're done
        for(int i = 0; i < activeConnections; i++)
        {
            int exitMethod;

            if(waitpid(connections[i], &exitMethod, WNOHANG))
            {
                connections[i] = connections[activeConnections - 1];
                activeConnections--;
                i--;
            }
        }

        // Accept a connection
        int connectionFd = accept(listenSocketFd, (struct sockaddr*)&clientAddr, &sizeOfClientInfo);

        if(connectionFd < 0)
        {
            perror("Error on accept");
            exit(1);
        }

        if(connectionVerified(connectionFd, 'E') && activeConnections < 5)
        {
            int childPid = acceptConnection(connectionFd);
            connections[activeConnections++] = childPid;
        }

        else
            sendData(connectionFd, "N", 1);
    }

    close(listenSocketFd);
    return 0;
}
