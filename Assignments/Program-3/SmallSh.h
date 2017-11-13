#pragma once

#include "Command.h"
#include "VectorInt.h"

int executeCommand(Command* cmd);

void checkBackgroundProcesses(VectorInt* bgProcesses);

int main();

