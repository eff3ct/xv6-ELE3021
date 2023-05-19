#include "types.h"
#include "defs.h"
#include "thread.h"

int
thread_create(thread_t* thread, void* (*start_rotine)(void *), void *arg)
{
    return 0;
}

void
thread_exit(void* ret_val)
{
    return;
}

int 
thread_join(thread_t thread, void** retval)
{
    return 0;
}

int
sys_thread_create(void)
{
    thread_t* thread;
    void* (*start_rotine)(void *);
    void* arg;
    if (argptr(0, (void*)&thread, sizeof(thread)) < 0 || argptr(1, (void*)&start_rotine, sizeof(start_rotine)) < 0 || argptr(2, (void*)&arg, sizeof(arg)) < 0)
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