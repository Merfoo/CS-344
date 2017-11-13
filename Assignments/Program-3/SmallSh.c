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
        // Input/Output file descriptors if needed
        int inputFd = -1;
        int outputFd = -1;

        // Create input file if required
        if(cmd->inputFile)
        {
            inputFd = open(cmd->inputFile, O_RDONLY);
           
            if(inputFd == -1)
            {
                perror("Error: ");
                exit(1);
            }

            // Set stdin to point to the input file
            dup2(inputFd, 0);
        }

        // Otherwise if its a background process, point stdin to /dev/null
        else if(cmd->foreground == false)
        {
            int nullInput = open("/dev/null", O_RDONLY);
            dup2(nullInput, 0);
        }

        // Create output file if required
        if(cmd->outputFile)
        {
            outputFd = open(cmd->outputFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            
            if(outputFd == -1)
            {
                perror("Error: ");
                exit(1);
            }

            // Set stdout to point to the output file
            dup2(outputFd, 1);
        }

        // Otherwise if its a background process, point stdout to /dev/null
        else if(cmd->foreground == false)
        {
            int nullOutput = open("/dev/null", O_WRONLY);
            dup2(nullOutput, 1);
        }

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

        // Parse command from input
        Command cmd;
        parseCommand(input, &cmd);
        free(input);

        // Check if the command is empty or begins/is a comment, if so do nothing
        if(cmd.cmd == NULL || cmd.cmd[0] == '#')
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
            fflush(stdout);
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

