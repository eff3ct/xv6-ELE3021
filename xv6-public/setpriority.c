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

void setPriority(int pid, int priority)
{
  if (priority < 0 || priority > 3) panic("setPriority: invalid priority.\n");
  acquire(&ptable.lock);
  struct proc *p;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->pid == pid) {
      p->priority = priority;
      break;
    }
  }
  release(&ptable.lock);
}

int sys_setPriority(void)
{
  int pid, priority;
  if(argint(0, &pid) < 0 || argint(1, &priority) < 0)
    return -1;
  setPriority(pid, priority);
  return 0;
}