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

/**
 * @brief 프로세스 테이블로부터 정보를 얻어와 pinfo_table에 저장합니다.
 * @param pinfo_table 
 */
void 
get_pinfo(void) 
{
  acquire(&ptable.lock);

  int i;
  struct proc *p;
  cprintf("\n[INFO] Running processes.\n");
  for(i = 0, p = ptable.proc; p < &ptable.proc[NPROC]; ++p, ++i) {
    // RUNNABLE 혹은 RUNNING인 프로세스들만 출력합니다.
    if(p->state == RUNNABLE || p->state == RUNNING) {
      cprintf("========================\n");
      cprintf("Name: %s\n", p->name);
      cprintf("pid: %d\n", p->pid);
      cprintf("The number of stack pages: %d\n", p->stack_size); // TODO: 스택 페이지 개수를 출력합니다.
      cprintf("Allocated memory: %d\n", p->sz);
      cprintf("Max limit of memory: %d\n", p->max_memory); // TODO: 최대 메모리 한도를 출력합니다.
    }
  }
  cprintf("========================\n\n"); 

  release(&ptable.lock);
}

int 
sys_get_pinfo(void) 
{
  get_pinfo();
  return 0;
}