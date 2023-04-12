#include "types.h"
#include "defs.h"
#include "spinlock.h"

// TODO : Implement schedulerLock and schedulerUnlock

extern int isSchedulerLocked;

/**
 * @brief Lock the MLFQ scheduler.
 * @param password certification for the locking.
 */
void 
schedulerLock(int password)
{

    if (password == 2021093054) {
        isSchedulerLocked = 1;
    }
    else {
        // TODO : Print current process' pid, time, quantum, level and kill.
    }
}

/**
 * @brief Unlock the MLFQ scheduler.
 * @param password certification for the unlocking.
 */
void
schedulerUnlock(int password)
{
    if (password == 2021093054) {
        isSchedulerLocked = 0;
    }
    else {
        // TODO : Print current process' pid, time, quantum, level and kill.
    }
}

// wrapper functions for above system calls.

int
sys_schedulerLock(void) 
{
    int password;
    if (argint(0, &password) < 0) return -1;
    schedulerLock(password);
    return 0;    
}

int
sys_schedulerUnlock(void)
{
    int password;
    if (argint(0, &password) < 0) return -1;
    schedulerUnlock(password);    
    return 0;    
}
