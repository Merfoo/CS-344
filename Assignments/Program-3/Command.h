#pragma once

#include <stdbool.h>

// Max input length and command args for a Command
#define MAX_INPUT_LENGTH 2049
#define MAX_COMMAND_ARGS 514

// Struct to hold information about a command
typedef struct
{
    // String used for the strtok function
    char strtokInput[MAX_INPUT_LENGTH];

    // Command to be run
    char* cmd;

    // Arguments for the command
    char* args[MAX_COMMAND_ARGS];
    
    // Amount of arguments for the command
    int argCount;

    // Input file for the command
    char* inputFile;

    // Ouput file for the command
    char* outputFile;

    // Type of command, true for foreground, false for background
    bool foreground;
} Command;

// Parses an input string into a Command object
void parseCommand(char* input, Command* cmd);

// Executes the command in a new process, returns the process id
int executeCommand(Command* cmd);
