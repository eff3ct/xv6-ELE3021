#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"

int
sync(void)
{
    // TODO: 싱크 구현
    return 0;
}

int
sys_sync(void)
{
    return sync();
}