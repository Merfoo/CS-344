#pragma once

// Return a string thats the encrypted version of the text
char* encryptText(char* text, int textLength, char* key);

// Forks the process, receives the unecrypted text from the accepted socket
// encrypts it, and returns it to the client
int acceptConnection(int connectionFd);

int main(int argc, char* argv[]);

