#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

extern struct proc* ext_initproc;

extern struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

extern void forkret(void);
extern void trapret(void);

static int thread_id = 0;

// ! thread_t는 uint 입니다.

// ! proc.c에서 가져온 함수입니다.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

/**
 * @brief 쓰레드 할당을 위한 내부함수 입니다. fork, allocproc의 코드를 참고하여 작성하였습니다.
 * @return struct proc*
 */
struct proc*
alloc_thread(void)
{
  struct proc* curproc = myproc();
  struct proc* p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->state == UNUSED) {
      // 스레드를 할당합니다.
      p->max_memory = 0;
      p->tid = ++thread_id;
      p->is_thread = 1;
      p->pid = myproc()->pid;
      p->state = EMBRYO;

      // 스레드의 부모를 설정합니다.
      p->parent = curproc;
      p->master = (curproc->is_thread == 1) ? curproc->master : curproc;

      // 커널 스택을 할당합니다.
      if((p->kstack = kalloc()) == 0) {
        p->state = UNUSED;
        return (void*)0;
      }

      // 스택 포인터 입니다.
      char* sp = p->kstack + KSTACKSIZE;

      // 트랩 프레임 크기 만큼 할당합니다.
      sp -= sizeof *p->tf;
      p->tf = (struct trapframe*)sp;

      // 스레드의 트랩 프레임을 설정합니다.
      *p->tf = *curproc->tf;

      sp -= 4;
      *(uint*)sp = (uint)trapret;

      // 컨텍스트 크기 만큼 할당합니다.
      sp -= sizeof *p->context;
      p->context = (struct context*)sp;
      memset(p->context, 0, sizeof *p->context);
      p->context->eip = (uint)forkret;

      // 페이지 테이블을 공유합니다.
      p->pgdir = curproc->pgdir;

      return p;
    }
  }

  return (void*)0;
}

/**
 * @brief 유저 스택을 할당합니다.
 * @param trd 
 * @return int 성공시 0, 실패시 -1을 반환합니다.
 */
int
alloc_ustack(struct proc* trd)
{
  // 스레드의 유저 스택을 설정합니다.
  trd->master->sz = PGROUNDUP(trd->master->sz);
  if((trd->master->sz = allocuvm(trd->master->pgdir, trd->master->sz, trd->master->sz + 2 * PGSIZE)) == 0) {
    kfree(trd->kstack);
    trd->kstack = 0;
    trd->state = UNUSED;
    return -1;
  }
  clearpteu(trd->master->pgdir, (char*)(trd->master->sz - 2 * PGSIZE));
  trd->sz = trd->master->sz;

  // cprintf("alloc_ustack: %d\n", trd->sz);
  // cprintf("master: %d\n", trd->master->sz);

  return 0;
}

/**
 * @brief 스레드를 만드는 함수입니다. start_routine(arg)로 실행합니다. fork, exec, allocproc등을 참고했습니다.
 * @param thread 
 * @param start_routine 
 * @param arg 
 * @return int 성공 시 0, 실패 시 -1을 반환합니다.
 */
int
thread_create(thread_t* thread, void* (*start_routine)(void *), void *arg)
{
  struct proc* curproc = myproc();
  struct proc* trd;

  acquire(&ptable.lock);
  // 스레드를 할당합니다. (fork에서 참고함)
  if((trd = alloc_thread()) == 0) {
    return -1;
  }
  // 유저 스택을 할당합니다. (exec에서 참고함)
  if(alloc_ustack(trd) == -1) {
    return -1;
  }

  // 스레드의 스택에 실행할 함수와 인자를 넣습니다.
  uint sp = trd->sz;
  uint ustack[2];
  ustack[0] = 0xffffffff;
  ustack[1] = (uint)arg;
  sp -= 2 * sizeof(uint);
  if(copyout(trd->pgdir, sp, ustack, 2 * sizeof(uint)) < 0) {
    return -1;
  }
  trd->tf->eip = (uint)start_routine;
  trd->tf->esp = sp;

  // 모든 스레드에서 sz를 공유합니다.
  for(struct proc* p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->pid == trd->pid) 
      p->sz = trd->sz;
  }

  release(&ptable.lock);

  // 파일 디스크립터를 복사합니다.
  for(int i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      trd->ofile[i] = filedup(curproc->ofile[i]);
  trd->cwd = idup(curproc->cwd);

  // 스레드의 이름을 설정합니다.
  safestrcpy(trd->name, curproc->name, sizeof(curproc->name));

  // 스레드를 준비 상태로 변경합니다.
  acquire(&ptable.lock);

  trd->state = RUNNABLE;
  *thread = trd->tid;

  release(&ptable.lock);

  return 0;   
}

/**
 * @brief 스레드가 종료되는 함수입니다. exit, wait, exec등을 참고했습니다.
 * @param ret_val 해당 스레드의 리턴값입니다.
 */
void
thread_exit(void* ret_val)
{
  struct proc* trd = myproc();
  struct proc* p;

  if(trd->is_thread == 0)
    panic("[ERROR] thread_exit: try to exit non-thread process.\n");

  if(trd == ext_initproc)
    panic("init exiting");

  // Close all open files.
  for(int fd = 0; fd < NOFILE; fd++){
    if(trd->ofile[fd]){
      fileclose(trd->ofile[fd]);
      trd->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(trd->cwd);
  end_op();
  trd->cwd = 0;
  
  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(trd->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == trd){
      p->parent = ext_initproc;
      if(p->state == ZOMBIE)
        wakeup1(ext_initproc);
    }
  }

  // Jump into the scheduler, never to return.
  trd->state = ZOMBIE;

  // 리턴값을 설정해줍니다.
  trd->ret_val = ret_val;
  sched();
  panic("zombie exit");
}

/**
 * @brief 스레드를 기다리는 함수입니다. wait, exit, exec등을 참고했습니다.
 * @param thread 기다릴 스레드입니다.
 * @param retval 스레드의 리턴값을 저장할 변수입니다.
 * @return int 
 */
int 
thread_join(thread_t thread, void** retval)
{
  struct proc *p;
  struct proc *trd = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->tid != thread)
        continue;
      if(p->state == ZOMBIE){
        // Found one.
        kfree(p->kstack);
        p->kstack = 0;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        // 스레드의 리턴값을 설정합니다.
        *retval = p->ret_val;
        // 스레드를 초기화합니다.
        p->tid = 0;
        p->is_thread = 0;
        p->master = 0;
        p->ret_val = 0;
        release(&ptable.lock);
        return 0;
      }
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(trd, &ptable.lock);  //DOC: wait-sleep
  }

  return -1;
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