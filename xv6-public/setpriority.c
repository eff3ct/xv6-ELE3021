#include "types.h"
#include "defs.h"

void setPriority(int pid, int priority)
{
    // TODO : set [pid] process' priority to [priority]
}

void sys_setPriority(int pid, int priority)
{
    setPriority(pid, priority);
}