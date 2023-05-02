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
  return myproc()->queue_level;
}

int
sys_getLevel(void) 
{
  return getLevel();
}