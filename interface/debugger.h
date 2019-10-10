#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "m64p_types.h"

void DebuggerStep();
void DebuggerSetupCallbacks();

void DebuggerSetRunState(m64p_dbg_runstate);
m64p_dbg_runstate DebuggerGetRunState();
int DebuggerGetInstruction(char* op, char* args, uint32_t addr = 0);
uint32_t DebuggerGetMemory(uint32_t addr, int size = 32);
int DebuggerSetBreakpoint(uint32_t addr, int type = 0);
int DebuggerGetBreakpoint(uint32_t addr, int type = 0);
void DebuggerRemoveBreakpoint(int index);
void DebuggerRemoveAllBreakpoints();

extern bool l_DebugLoopWait;
extern uint32_t l_DebugProgramCounter;
extern int l_DebugStepsPending;

#endif
