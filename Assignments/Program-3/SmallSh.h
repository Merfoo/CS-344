#pragma once

#include "stdbool.h"
#include "Command.h"
#include "VectorInt.h"

// Current foreground mode status, vector of background process ids
bool foregroundMode;
VectorInt bgProcesses;

// Returns a dynamically allocated string that has all ocurrences of the 
// target substring with the replacement substring
char* replaceStr(char* source, char* target, char* substitute);

// Executes a command from a command struct
int executeCommand(Command* cmd);

// Checks if any of the background processes has ended
void checkBackgroundProcesses();

// Flip foreground mode when the CTRL-Z command is sent
void flipForegroundMode(int sig);

// Entry point of smallsh
int main();

