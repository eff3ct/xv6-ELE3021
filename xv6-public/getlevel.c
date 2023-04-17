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

int
getLevel(void)
{
  int ret = -1;
  acquire(&ptable.lock);
  ret = myproc()->queue_level;
  release(&ptable.lock);
  return ret;
}

int
sys_getLevel(void) 
{
  return getLevel();
}