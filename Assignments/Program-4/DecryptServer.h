#pragma once

// Return a string thats the decrypted version of the text
char* decryptText(char* text, int textLength, char* key);

// Forks the process, receives the encrypted text from the accepted socket
// decrypts it, and returns it to the client
int acceptConnection(int connectionFd);

int main(int argc, char* argv[]);

