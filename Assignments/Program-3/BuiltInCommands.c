#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "BuiltInCommands.h"

// Used for the last processes exist status/terminating signal
int lastExitStatus = 0;
int lastTermSignal = 0;

// Exits the program, stops all background processes
void cmdExit(VectorInt* bgProcesses)
{
    // Loop through each background process, killing them
    for(int i = 0; i < bgProcesses->size; i++)
    {
        // Send terminate signal
        kill(bgProcesses->array[i], SIGTERM);

        // Wait for the process to terminate
        int exitMethod;
        waitpid(bgProcesses->array[i], &exitMethod, 0);
    }

    // Exit
    exit(0);
}

// Changes to the specified directory, if NULL, changes to the
// path specified in the HOME environment variable
void cmdCd(char* path)
{
    if(path == NULL)
        path = getenv("HOME");

    chdir(path);
}

// Prints the last process exit status or terminating signal
void cmdStatus()
{
    if(lastExitStatus >= 0)
        printf("exit value %d\n", lastExitStatus);

    else
        printf("terminated by signal %d\n", lastTermSignal);
}

