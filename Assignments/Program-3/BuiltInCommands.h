#pragma once

#include "VectorInt.h"

#define CMD_EXIT "exit"
#define CMD_CD "cd"
#define CMD_STATUS "status"

// Last exit status/terminate signal
extern int lastExitStatus;
extern int lastTermSignal;

// Kills all background processes and exits
void cmdExit(VectorInt* bgProcesses);

// Changes the directory
void cmdCd(char* path);

// Displays last commands exist status/terminate signal
extern void cmdStatus();

