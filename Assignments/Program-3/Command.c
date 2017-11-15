// GCC has bug for things being initialized to {0} in c99, so ignore those warnings
#pragma GCC diagnostic ignored "-Wmissing-braces"

// To make some of the functions work in c99
#define _GNU_SOURCE

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
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
            // If the beginning of the first token is a '#', then this input is a comment
            // thus, there will be no command
            if(token[0] == '#')
                break;

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

// Run the command in a new process
int executeCommand(Command* cmd)
{
    // PID of the child
    pid_t childPid = -1;

    // Fork a new process
    childPid = fork();    

    // If childPID is 0, we're the child process
    if(childPid == 0)
    {
        // Create input file if required
        if(cmd->inputFile)
        {
            int inputFd = open(cmd->inputFile, O_RDONLY);
           
            if(inputFd == -1)
            {
                perror(cmd->inputFile);
                exit(1);
            }

            // Set stdin to point to the input file
            dup2(inputFd, 0);
        }

        // Create output file if required
        if(cmd->outputFile)
        {
            int outputFd = open(cmd->outputFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            
            if(outputFd == -1)
            {
                perror(cmd->outputFile);
                exit(1);
            }

            // Set stdout to point to the output file
            dup2(outputFd, 1);
        }

        // If its a foreground process we need to set the SIGINT signal handler
        // back to the default
        if(cmd->foreground)
        {
            struct sigaction defaultAction = {0};
            defaultAction.sa_handler = SIG_DFL;
            sigaction(SIGINT, &defaultAction, NULL);
        }

        // Else its a background process, set its stdin/stdout to null if 
        // file redirections aren't set
        else
        {
            // Redirect stdin/stout if not set
            if(cmd->inputFile == NULL)
            {
                int nullInput = open("/dev/null", O_RDONLY);
                dup2(nullInput, 0);   
            }

            if(cmd->outputFile == NULL)
            {
                int nullOutput = open("/dev/null", O_WRONLY);
                dup2(nullOutput, 1);
            }
        }

        // Replace the current process with the wanted command
        if(execvp(cmd->cmd, cmd->args) < 0)
        {
            perror(cmd->cmd);
            exit(1);
        }
    }

    // Otherwise we're the parent
    return childPid;
}
