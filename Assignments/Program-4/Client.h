#pragma once

// Gets the content from a file and sets the length to the variable passed in
char* getFileContent(char* filename, int* length);

// Checks if the string contains any characters not within A-Z or space
// Returns 1 if contains bad characters
// Returns 0 otherwise
int validFileContent(char* string, int length);

