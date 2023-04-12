#include "types.h"
#include "defs.h"

int
getLevel(void)
{
    // TODO : implement 'return a queue level which the current process is in.'
}

int
sys_getLevel(void) 
{
    return getLevel();
}