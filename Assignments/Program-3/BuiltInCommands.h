#pragma once

#define CMD_EXIT "exit"
#define CMD_CD "cd"
#define CMD_STATUS "status"

extern int lastExitStatus;
extern int lastTermSignal;

void cmdExit();

void cmdCd(char* path);

extern void cmdStatus();

