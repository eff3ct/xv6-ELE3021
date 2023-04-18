#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

extern struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;
extern struct proc_queue sched_lk_q;
extern struct proc_queue L0;
extern struct spinlock tickslock;
extern uint ticks;

extern int isSchedulerLocked;

/**
 * @brief Lock the MLFQ scheduler.
 * @param password certification for the locking.
 */
void 
schedulerLock(int password)
{
  if (isSchedulerLocked == 1) 
    panic("schedulerLock: scheduler is already locked.\n");
  if (password == 2021093054) {
    isSchedulerLocked = 1;
    acquire(&tickslock);
    ticks = 0;
    wakeup(&ticks);
    release(&tickslock);

    acquire(&ptable.lock);
    push_proc(&sched_lk_q, myproc());
    release(&ptable.lock);
  }
  else {
    cprintf("\n----------------------------------------\n");
    cprintf("[ERROR] Scheduler lock failed. Killing current process.\n");
    cprintf("[INFO] pid: %d, quantum: %d, level: %d\n", myproc()->pid, 2 * myproc()->queue_level + 4 - myproc()->run_ticks, myproc()->queue_level);
    cprintf("----------------------------------------\n\n");
  }
}

/**
 * @brief Unlock the MLFQ scheduler.
 * @param password certification for the unlocking.
 */
void
schedulerUnlock(int password)
{
  if (isSchedulerLocked == 0) 
    panic("schedulerUnlock: scheduler is not locked.\n");
  if (password == 2021093054) {
    isSchedulerLocked = 0;
    acquire(&ptable.lock);

    clear_queue(&sched_lk_q);
    myproc()->priority = 3;
    push_proc(&L0, myproc());

    release(&ptable.lock);
  }
  else {
    cprintf("\n----------------------------------------\n");
    cprintf("[ERROR] Scheduler lock failed. Killing current process.\n");
    cprintf("[INFO] pid: %d, quantum: %d, level: %d\n", myproc()->pid, 2 * myproc()->queue_level + 4 - myproc()->run_ticks, myproc()->queue_level);
    cprintf("----------------------------------------\n\n");
    kill(myproc()->pid);
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
