#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "Command.h"

// Parses an input string into a Command object
void parseCommand(char* input, Command* cmd)
{
    // Set the strtokInput to nulls and copy the input to it
    memset(cmd->strtokInput, '\0', MAX_INPUT_LENGTH);
    strncpy(cmd->strtokInput, input, MAX_INPUT_LENGTH - 1);

    // Initialize the cmd object
    cmd->cmd = NULL;
    cmd->argCount = 0;
    cmd->inputFile = NULL;
    cmd->outputFile = NULL;
    cmd->foreground = true;

    // Begin parsing the string by tokenizing it with spaces " "
    char* token = strtok(cmd->strtokInput, " ");
    bool tokenInputFile = false;
    bool tokenOutputFile = false; 

    while(token)
    {
        // If there are tokens, that means the previous "&" is part of the argument
        cmd->foreground = true;

        // If its the first token, this token is the command and first argument
        if(cmd->argCount == 0)
        {
            cmd->cmd = token;
            cmd->args[cmd->argCount] = token;
            cmd->argCount++;
        }

        // Otherwise, the token could be another argument or file indirections
        else
        {
            // If the token is "<" then the next token is the input filename
            if(strcmp(token, "<") == 0)
                tokenInputFile = true;    

            // If the token is ">" then the next token is the output filename
            else if(strcmp(token, ">") == 0)
                tokenOutputFile = true;

            // If tokenInputFile is true then the token is the input filename
            else if(tokenInputFile)
            {
                cmd->inputFile = token;
                tokenInputFile = false;
            }

            // If tokenOutputFile is true then the token is the output filename
            else if(tokenOutputFile)
            {
                cmd->outputFile = token;
                tokenOutputFile = false;
            }

            // Else its probably an argument to the command
            else
            {
                if(strcmp(token, "&") == 0)
                    cmd->foreground = false;

                cmd->args[cmd->argCount] = token;
                cmd->argCount++;
            }
        }

        // If the command is a background aka foreground is false then remove
        // the & from the argument list
        if(cmd->foreground == false)
            cmd->argCount--;

        // Get the next token
        token = strtok(NULL, " ");
    }

    // Set the last argument to null
    cmd->args[cmd->argCount] = '\0';
}

// Prints the contents of a Command object for debugging
void printCommand(Command* cmd)
{
    printf("Command: %s\n", cmd->cmd);
    printf("Args:\n");

    for(int i = 0; i < cmd->argCount; i++)
    {
        printf("\t%s\n", cmd->args[i]);
    }

    printf("InputFile: %s\n", cmd->inputFile);
    printf("OutputFile: %s\n", cmd->outputFile);
    printf("Foreground: %d\n", cmd->foreground);
}

