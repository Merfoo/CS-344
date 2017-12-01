// So that getline works with c99
#define _GNU_SOURCE

#include "Client.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
int validFileContent(char* string, int length)
{
    for(int i = 0; i < length; i++)
        if(string[i] != ' ' && (string[i] < 'A' || string[i] > 'Z'))
            return 0;

    return 1;
}

