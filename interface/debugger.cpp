#include "debugger.h"
#include "core_interface.h"
#include "common.h"

uint32_t l_DebugProgramCounter = 0;
int l_DebugStepsPending = 0;
bool l_DebugLoopWait = false;

void DebuggerStep()
{
    m64p_error result = (*DebugStep)();
    if(result != M64ERR_SUCCESS)
        DebugMessage(M64MSG_WARNING, "Failed to step debugger.");
}

void DebuggerInit()
{
    DebuggerSetRunState(M64P_DBG_RUNSTATE_RUNNING);
}

void DebuggerUpdate(uint32_t pc)
{
    l_DebugProgramCounter = pc;

    if(!l_DebugStepsPending)
    {
        l_DebugLoopWait = false;
        DebuggerSetRunState(M64P_DBG_RUNSTATE_PAUSED);
    }
    else
    {
        --l_DebugStepsPending;
        DebuggerStep();
    }
}

void DebuggerVi()
{

}

void DebuggerSetupCallbacks()
{
    m64p_error result = (*DebugSetCallbacks)(DebuggerInit, DebuggerUpdate, DebuggerVi);

    if(result != M64ERR_SUCCESS)
        DebugMessage(M64MSG_WARNING, "Failed to set debug callbacks.");
}

void DebuggerSetRunState(m64p_dbg_runstate state)
{
    m64p_error result = (*DebugSetRunState)(state);

    if(result != M64ERR_SUCCESS)
        DebugMessage(M64MSG_WARNING, "Failed to set debug run state.");
    else if(state == M64P_DBG_RUNSTATE_RUNNING)
        DebuggerStep();
}

m64p_dbg_runstate DebuggerGetRunState()
{
    return static_cast<m64p_dbg_runstate>((*DebugGetState)(M64P_DBG_RUN_STATE));
}

int DebuggerGetInstruction(char op[64], char args[64], uint32_t addr)
{
    uint32_t lookupAddr, lookupData;
    lookupAddr = (addr ? addr : l_DebugProgramCounter);
    lookupAddr &= ~0x03;
    lookupData = (*DebugMemRead32)(lookupAddr);
    (*DebugDecodeOp)(lookupData, op, args, lookupAddr);
    if(op[0] == 'b' || op[0] == 'j')
        return 2;
    return 1;
}
