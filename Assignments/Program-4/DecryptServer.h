#pragma once

char* encryptText(char* text, int textLength, char* key);

int acceptConnection(int connectionFd);

int main(int argc, char* argv[]);

