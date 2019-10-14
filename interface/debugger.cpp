#include "debugger.h"
#include "core_interface.h"
#include "common.h"

uint32_t l_DebugProgramCounter = 0;
int l_DebugStepsPending = 0;
bool l_DebugLoopWait = false;
static m64p_breakpoint *l_DebugBreakpoints = nullptr;
static int l_DebugNumBreakpoints = 0;

void DebuggerStep()
{
    m64p_error result = (*DebugStep)();
    if(result != M64ERR_SUCCESS)
        DebugMessage(M64MSG_WARNING, "Failed to step debugger.");
}

void DebuggerInit()
{
    DebuggerSetRunState(M64P_DBG_RUNSTATE_RUNNING);
    if(l_DebugBreakpoints != nullptr)
    {
        if(l_DebugNumBreakpoints != 0)
            DebuggerRemoveAllBreakpoints();
        delete l_DebugBreakpoints;
    }
    l_DebugBreakpoints = static_cast<m64p_breakpoint*>(malloc(BREAKPOINTS_MAX_NUMBER * sizeof(m64p_breakpoint)));
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

uint64_t DebuggerGetMemory(uint32_t addr, int size)
{
    if(size == 32)
    {
        return (*DebugMemRead32)(addr);
    }
    else if(size == 64)
    {
        return (*DebugMemRead64)(addr);
    }
    else
    {
        DebugMessage(M64MSG_WARNING, "Memory size not implemented.");
        return 0x0;
    }
}

int DebuggerSetBreakpoint(uint32_t addr, m64p_dbg_bkp_flags flags)
{
    m64p_breakpoint breakpoint;
    if(flags & (M64P_BKP_FLAG_READ | M64P_BKP_FLAG_WRITE) && addr >= 0x80000000 && addr < 0x90000000)
    {
        breakpoint.address = addr - 0x80000000;
        breakpoint.endaddr = addr - 0x80000000;
    }
    else
    {
        breakpoint.address = addr;
        breakpoint.endaddr = addr;
    }
    breakpoint.flags = (M64P_BKP_FLAG_ENABLED | flags);

    int numBreakpoints = (*DebugBreakpointCommand)(M64P_BKP_CMD_ADD_STRUCT, 0, &breakpoint);
    if(numBreakpoints == -1)
        return -1;
    
    l_DebugBreakpoints[l_DebugNumBreakpoints] = breakpoint;
    l_DebugNumBreakpoints++;
    return l_DebugNumBreakpoints - 1;
}

int DebuggerGetBreakpoint(uint32_t addr, m64p_dbg_bkp_flags flags)
{
    flags = m64p_dbg_bkp_flags(flags | M64P_BKP_FLAG_ENABLED); // Disabling breakpoints not implemented
    if(flags == M64P_BKP_FLAG_ENABLED)
    {
        uint32_t memAddr = addr;
        if(addr >= 0x80000000 && addr < 0x90000000)
            memAddr = addr - 0x80000000;
        for(int i = 0; i < l_DebugNumBreakpoints; i++)
        {
            if(l_DebugBreakpoints[i].address == memAddr)
                return l_DebugBreakpoints[i].flags;
        }
    }
    else if(flags & M64P_BKP_FLAG_EXEC)
    {
        for(int i = 0; i < l_DebugNumBreakpoints; i++)
        {
            if(l_DebugBreakpoints[i].address == addr
            && l_DebugBreakpoints[i].flags == m64p_dbg_bkp_flags(flags | M64P_BKP_FLAG_ENABLED))
                return i;
        }
    }
    else
    {
        uint32_t memAddr = addr;
        if(addr >= 0x80000000 && addr < 0x90000000)
            memAddr = addr - 0x80000000;
        for(int i = 0; i < l_DebugNumBreakpoints; i++)
        {
            if(l_DebugBreakpoints[i].address == memAddr)
                return i;
        }
    }

    return -1;
}

void DebuggerRemoveBreakpoint(int index)
{
    if(index < 0 || index >= l_DebugNumBreakpoints)
        return;
    (*DebugBreakpointCommand)(M64P_BKP_CMD_REMOVE_IDX, index, NULL);
    l_DebugNumBreakpoints--;

    for (int j = index + 1; j < l_DebugNumBreakpoints; j++) {
        l_DebugBreakpoints[j - 1] = l_DebugBreakpoints[j];
    }
}

void DebuggerRemoveAllBreakpoints()
{
    if(l_DebugNumBreakpoints <= 0)
        return;
    for(int i = l_DebugNumBreakpoints; i > 0; i--)
        (*DebugBreakpointCommand)(M64P_BKP_CMD_REMOVE_IDX, i, NULL);
    l_DebugNumBreakpoints = 0;
}
