int
setmemorylimit(int pid, int limit)
{
    return 0;
}

int
sys_setmemorylimit(void)
{
    int pid, limit;
    if (argint(0, &pid) < 0 || argint(1, &limit) < 0)
        return -1;
    return setmemorylimit(pid, limit);
}