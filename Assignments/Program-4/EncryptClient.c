
// So that getline works with c99
#define _GNU_SOURCE

// For c99 h_addr is h_addr_list[0]
#define h_addr h_addr_list[0]

#include "EncryptClient.h"
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

char* getFileContent(char* filename, int* length)
{
    // Open the file for reading
    FILE* f = fopen(filename, "r");

    // Print error message if couldn't open file
    if(f == NULL)
    {
        perror(filename);
        exit(1);
    }

    // Get a line from the file and return the string
    char* data = NULL;
    size_t dataSize = 0;
    getline(&data, &dataSize, f);

    // Remove the newline character and calculate the length
    *length = strlen(data) - 1;
    data[*length] = '\0';

    // Close the file
    fclose(f);

    return data;
}

// Checks if the string contains any characters not within A-Z or space
// Returns 1 if contains bad characters
// Returns 0 otherwise
int containsBadCharacters(char* string, int length)
{
    for(int i = 0; i < length; i++)
        if(string[i] != ' ' && (string[i] < 'A' || string[i] > 'Z'))
            return 1;

    return 0;
}

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
    if(containsBadCharacters(text, textLength))
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

    printf("%d\n", containsBadCharacters(text, textLength));
    printf("%s\n", text);
    printf("%s\n", key);

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

    // Close socket
    close(socketFd);
    return 0;
}

