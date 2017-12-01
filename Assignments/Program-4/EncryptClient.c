// So that getline works with c99
#define _GNU_SOURCE

#include "EncryptClient.h"
#include "Client.h"
#include "SocketUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char* argv[])
{
    // Exit if plaintext, key, or port wasn't passed in
    if(argc < 4)
    {
        fprintf(stderr, "USAGE: %s plaintext key port\n", argv[0]);
        exit(0);
    }

    // Get args data
    char* textFilename = argv[1];
    char* keyFilename = argv[2];
    int port = atoi(argv[3]);

    // Data for the plaintext and key files
    int textLength = 0;
    int keyLength = 0;    
    char* text = getFileContent(textFilename, &textLength);
    char* key = getFileContent(keyFilename, &keyLength);

    // Exit if the plaintext contains bad characters
    if(!validFileContent(text, textLength))
    {
        fprintf(stderr, "otp_enc error: input contains bad characters\n");
        exit(1);
    }

    // Exit if the key is shorter than the plaintext
    if(keyLength < textLength)
    {
        fprintf(stderr, "Error: key '%s' is too short\n", keyFilename);
        exit(1);
    }

    // Create client socket 
    struct sockaddr_in serverAddr;
    int socketFd = createClientSocket(port, &serverAddr);
   
    // Connect to server
    if(connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("CLIENT: ERROR connection");
        exit(1);
    }

    // If the connection got rejected by the server it means we connected
    // to the wrong server
    if(!connectionAccepted(socketFd, "E"))
    {
        fprintf(stderr, "Error: could not contact otp_enc_d on port %d\n", port);
        exit(0);
    }

    // Send the plaintext by first sending its length and then the contents
    sendMessageLength(socketFd, textLength);
    sendData(socketFd, text, textLength);

    // Send the key by first sending it length and then the contents
    sendMessageLength(socketFd, keyLength);
    sendData(socketFd, key, keyLength);

    // Receive the encrypted text and display it
    recvData(socketFd, text, textLength);
    printf("%s\n", text);

    // Free up the memory allocated for the text and key    
    free(text);
    free(key);

    // Close socket
    close(socketFd);
    return 0;
}

