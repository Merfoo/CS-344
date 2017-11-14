#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "SmallSh.h"
#include "Command.h"
#include "BuiltInCommands.h"
#include "VectorInt.h"

// Returns a dynamically allocated string that has all ocurrences of the 
// target substring with the replacement substring
char* replaceStr(char* source, char* target, char* substitute)
{
    // Sizes for strings
    int sourceSize = strlen(source);
    int targetSize = strlen(target);
    int substituteSize = strlen(substitute);
    int newStrSize = 0;
        
    // Create array that wil contain indices of the source to copy to
    // the new one
    int* sourceIndices = malloc((sourceSize * sizeof(int)) * 2);
    int indicesCount = 0;

    // Loop over the source, add the start/end index to sourceIndices, 
    // or -1 if the substring was found
    int begIndex = 0;

    while(begIndex < sourceSize)
    {
        // Find index of target string in source string
        char* substr = strstr(&source[begIndex], target);

        // If a substring is found we need to add the indices of the substring of
        // the source before the substring and add -1 to the indicies indicate that
        // the substitute string should be added
        if(substr)
        {
            // Calculate start index of substring
            int substrIndex = substr - source;
           
            // If the substring index is farther than our current index
            // we need to add the substring from our current index
            // to the index of the substring 
            if(substrIndex != begIndex)
            {
                newStrSize += substrIndex - begIndex;
                sourceIndices[indicesCount++] = begIndex;
                sourceIndices[indicesCount++] = substrIndex;
            }

            // Add -1 to indicate that the substitute string should be added            
            sourceIndices[indicesCount++] = -1;
            newStrSize += substituteSize;
            begIndex = substrIndex + targetSize;
        }

        // Else, no more substrings were found, so add the substring from the
        // current index to the end of the string to the indices
        else
        {
            newStrSize += sourceSize - begIndex;
            sourceIndices[indicesCount++] = begIndex;
            sourceIndices[indicesCount++] = sourceSize;
            break;  
        }
    }

    // Begin creating the new string to contain the replaced substrings,
    // plus one more for null
    char* newStr = malloc((newStrSize * sizeof(char)) + 1);
    int newStrIndex = 0;

    // Loop over each indice
    for(int i = 0; i < indicesCount; i++)
    {
        // If -1, add the substitute string
        if(sourceIndices[i] == -1)
        {
            strncpy(&newStr[newStrIndex], substitute, substituteSize);
            newStrIndex += substituteSize;
        }

        // Else, add the substring from the beg index sourceIndices[i]
        // to the end index sourceIndices[i + 1]
        else
        {
            int copySize = sourceIndices[i + 1] - sourceIndices[i];
            strncpy(&newStr[newStrIndex], &source[sourceIndices[i]], copySize);
            newStrIndex += copySize;
            i++;
        }
    }

    // Add the null character at last spot
    newStr[newStrSize] = '\0';

    return newStr;
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
                perror("Error: ");
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
                perror("Error: ");
                exit(1);
            }

            // Set stdout to point to the output file
            dup2(outputFd, 1);
        }

        // If its a background process, set its stdin/stdout to null if 
        // file redirections aren't set
        if(cmd->foreground == false)
        {
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
            perror("Error: ");
            exit(1);
        }
    }

    // Otherwise we're the parent
    return childPid;
}

// Checks if any background processes ended
void checkBackgroundProcesses(VectorInt* bgProcesses)
{
    // Loop over each pid checking if it has been terminated
    for(int i = 0; i < bgProcesses->size; i++)
    {
        pid_t pid = bgProcesses->array[i];
        int exitMethod;

        // If the pid has been terminated, waitpid returns != 0
        if(waitpid(pid, &exitMethod, WNOHANG))
        {
            printf("background pid %d is done: ", pid);

            if(WIFEXITED(exitMethod))
                printf("exit value %d\n", WEXITSTATUS(exitMethod));

            else
                printf("terminated by signal %d\n", WTERMSIG(exitMethod));

            // Make sure the output from previous printfs get written
            fflush(stdout);

            // Remove the pid from the array by replacing it with the last pid in the array
            bgProcesses->array[i] = bgProcesses->array[bgProcesses->size - 1];
            bgProcesses->size--;
            i--;
        }
    }
}

int main()
{
    // Init last exit status/terminate signal
    lastExitStatus = -1;
    lastTermSignal = -1;

    // Create array to store background processes
    VectorInt bgProcesses;
    initVectorInt(&bgProcesses);

    // This processes pid for replacing $$ with the process id later on
    char strPid[10];
    sprintf(strPid, "%d", getpid());

    // Forever ask the user for input
    while(true)
    {
        // Check if any background process terminated
        checkBackgroundProcesses(&bgProcesses);

        // Display prompt
        printf(": ");
        fflush(stdout);

        // Get user input for the next command
        char* input = NULL;
        size_t bufSize = 0; 
        getline(&input, &bufSize, stdin);
        input[strlen(input) - 1] = '\0';

        // Expand "$$" to this processes pid
        char* expandedInput = replaceStr(input, "$$", strPid);
        free(input);

        // Parse command from input
        Command cmd;
        parseCommand(expandedInput, &cmd);
        free(expandedInput);

        // Check if the command is empty, if so do nothing
        if(cmd.cmd == NULL)
            continue;
 
        // Check if the command is one of our built-ins
        if(strcmp(cmd.cmd, CMD_EXIT) == 0)
            cmdExit();

        else if(strcmp(cmd.cmd, CMD_CD) == 0)
            cmdCd(cmd.args[1]);

        else if(strcmp(cmd.cmd, CMD_STATUS) == 0)
            cmdStatus();

        // Run the command in a new process
        else
        {
            // Run the command
            pid_t childPid = executeCommand(&cmd);

            // If its a background process, add the child pid to array of background processes
            if(cmd.foreground == 0)
            {
                addToVectorInt(&bgProcesses, childPid);

                // Display the background process info
                printf("background pid is %d\n", childPid);
                fflush(stdout);
            }

            // Else its a foreground process, so wait for it
            else
            {
                // Wait for the foreground process, get its exit method
                int exitMethod;
                waitpid(childPid, &exitMethod, 0);

                // Update the last exit status/terminate signal
                lastExitStatus = -1;
                lastTermSignal = -1;

                // Check if terminated
                if(WIFEXITED(exitMethod))
                    lastExitStatus = WEXITSTATUS(exitMethod);

                // Else signal terminated it
                else
                    lastTermSignal = WTERMSIG(exitMethod);
            }
        }
    }

    return 0;
}

