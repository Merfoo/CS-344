#include "DecryptServer.h"
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

// Return a string thats the decrypted version of the text
char* decryptText(char* text, int textLength, char* key)
{
    // Create string for the decrypted text
    char* decText = malloc((textLength + 1) * sizeof(char));

    // Loop over each text character
    for(int i = 0; i < textLength; i++)
    {
        // Subtract 'A' from the text and key to put it within
        // the [0, 26] bounds
        char textChar = text[i] - 'A';
        char keyChar = key[i] - 'A';

        // Convert spaces to 26
        if(text[i] == ' ')
            textChar = 26;

        if(key[i] == ' ')
            keyChar = 26;
        
        // Get the difference between the text and the key
        decText[i] = (textChar - keyChar);
        
        // Add 27 to the difference if its negative
        if(decText[i] < 0)
            decText[i] += 27;

        // Convert character to space
        if(decText[i] == 26)
            decText[i] = ' ';

        // Otherwise just add 'A' to get the character
        else
            decText[i] += 'A';
    }

    // Add the null terminating character and return the string
    decText[textLength] = '\0';
    return decText;
}

// Forks the process, receives the encrypted text from the accepted socket
// decrypts it, and returns it to the client
int acceptConnection(int connectionFd)
{
    // Fork the process
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

        // Decrypt the message and send it back
        char* dec = decryptText(text, textLength, key);
        sendData(connectionFd, dec, textLength);

        // Free all the strings allocated for the data and exit
        free(text);
        free(key);
        free(dec);
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

            // Wait for each process in case it finished
            if(waitpid(connections[i], &exitMethod, WNOHANG))
            {
                // Replace the finished process id with the last element
                // in the connections array and decrement the size
                connections[i] = connections[activeConnections - 1];
                activeConnections--;
                i--;
            }
        }

        // Accept a connection
        int connectionFd = accept(listenSocketFd, (struct sockaddr*)&clientAddr, &sizeOfClientInfo);

        // Exit and diplay error if connection failed
        if(connectionFd < 0)
        {
            perror("Error on accept");
            exit(1);
        }

        // Only accept if its been verified and there aren't too many concurrent connections
        if(connectionVerified(connectionFd, 'D') && activeConnections < MAX_CONCURRENT_CONNECTIONS)
        {
            int childPid = acceptConnection(connectionFd);
            connections[activeConnections++] = childPid;
        }

        // Else, reject the connection
        else
            sendData(connectionFd, "N", 1);
    }

    // Close the socket
    close(listenSocketFd);
    return 0;
}

