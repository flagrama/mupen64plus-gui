#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "m64p_types.h"

void DebuggerStep();
void DebuggerSetupCallbacks();

void DebuggerSetRunState(m64p_dbg_runstate);
m64p_dbg_runstate DebuggerGetRunState();
int DebuggerGetInstruction(char* op, char* args, uint32_t addr = 0);

extern bool l_DebugLoopWait;
extern uint32_t l_DebugProgramCounter;
extern int l_DebugStepsPending;

#endif
