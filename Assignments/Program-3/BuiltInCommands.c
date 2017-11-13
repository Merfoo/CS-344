#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "BuiltInCommands.h"

int lastExitStatus = 0;
int lastTermSignal = 0;

void cmdExit()
{
    exit(0);
}

void cmdCd(char* path)
{
    if(path == NULL)
        path = getenv("HOME");

    chdir(path);
}

void cmdStatus()
{
    if(lastExitStatus >= 0)
        printf("exit value %d\n", lastExitStatus);

    else
        printf("terminated by signal %d\n", lastTermSignal);
}

