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

extern void forkret(void);
extern void trapret(void);

static int thread_id = 0;

// ! thread_t는 struct proc와 완전히 같은 구조체입니다.
/**
 * @brief 쓰레드 할당을 위한 내부함수 입니다. fork, allocproc의 코드를 참고하여 작성하였습니다.
 * @return thread_t* 
 */
thread_t*
alloc_thread(void)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->state == UNUSED) {
      p->max_memory = 0;
      p->tid = ++thread_id;
      p->is_thread = 1;
      p->pid = myproc()->pid;
      p->state = EMBRYO;

      release(&ptable.lock);

      // 스택을 할당합니다.
      if((p->kstack = kalloc()) == 0) {
        p->state = UNUSED;
        return (void*)0;
      }

      // 스택 포인터 입니다.
      char* sp = p->kstack + KSTACKSIZE;

      // 트랩 프레임 크기 만큼 할당합니다.
      sp -= sizeof *p->tf;
      p->tf = (struct trapframe*)sp;

      sp -= 4;
      *(uint*)sp = (uint)trapret;

      // 컨텍스트 크기 만큼 할당합니다.
      sp -= sizeof *p->context;
      p->context = (struct context*)sp;
      memset(p->context, 0, sizeof *p->context);
      p->context->eip = (uint)forkret;

      return p;
    }
  }
  release(&ptable.lock);

  return (void*)0;
}

int
thread_create(thread_t* thread, void* (*start_routine)(void *), void *arg)
{
  struct proc* curproc = myproc();

  if((thread = alloc_thread()) == 0) {
    return -1;
  }

  // 프로세스 상태를 복사합니다.
  // TODO: 아래 내용을 바꿀 필요가 있음.
  if((thread = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(thread->kstack);
    thread->kstack = 0;
    thread->state = UNUSED;
    return -1;
  }
  thread->pgdir = curproc->pgdir;
  thread->sz = curproc->sz;
  thread->parent = curproc;
  *thread->tf = *curproc->tf;
  thread->stack_size = curproc->stack_size;

  // start_routine(arg)로 eip를 설정합니다. 
  thread->tf->eax = 0;
  thread->tf->eip = (uint)start_routine;
  thread->tf->esp = thread->sz - 4;
  *(uint*)(thread->tf->esp) = (uint)arg;

  for(int i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      thread->ofile[i] = filedup(curproc->ofile[i]);
  thread->cwd = idup(curproc->cwd);

  safestrcpy(thread->name, curproc->name, sizeof(curproc->name));

  // 스레드를 준비 상태로 변경합니다.
  acquire(&ptable.lock);

  thread->state = RUNNABLE;

  release(&ptable.lock);

  return 0;   
}

void
thread_exit(void* ret_val)
{
  struct proc* curproc = myproc();
  if(curproc->is_thread == 0)
    panic("[ERROR] thread_exit: try to exit non-thread process.\n");
  
  // 스레드가 종료되었음을 알립니다.
  curproc->state = ZOMBIE;

  // TODO: exit 작성

  return; 
}

int 
thread_join(thread_t thread, void** retval)
{
  // TODO: join 작성

  return 0;
}

int
sys_thread_create(void)
{
  thread_t* thread;
  void* (*start_rotine)(void *);
  void* arg;
  if(argptr(0, (void*)&thread, sizeof(thread)) < 0 || argptr(1, (void*)&start_rotine, sizeof(start_rotine)) < 0 || argptr(2, (void*)&arg, sizeof(arg)) < 0)
    return -1;
  return thread_create(thread, start_rotine, arg);
}

int
sys_thread_exit(void)
{
  void* ret_val;
  if (argptr(0, (void*)&ret_val, sizeof(ret_val)) < 0)
    return -1;
  thread_exit(ret_val);
  return 0;
}

int
sys_thread_join(void)
{
  thread_t thread;
  void** retval;
  if (argint(0, (int*)&thread) < 0 || argptr(1, (void*)&retval, sizeof(retval)) < 0)
    return -1;
  return thread_join(thread, retval);
}