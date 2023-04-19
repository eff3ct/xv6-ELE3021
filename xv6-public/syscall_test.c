#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char* argv[])
{
    if (argc <= 1) exit();
    const char* cmds[] = {
        "lock", 
        "set", 
        "get",
        "intlock",
        "intunlock"
    };

    printf(1, "argv[1]: %s\n", argv[1]);

    if (strcmp(argv[1], cmds[0]) == 0) {
        if (argc <= 2) exit();
        int password = atoi(argv[2]);
        schedulerLock(password);

        // a arbitrary task
        int d = 1;
        for (int i = 0; i < 90000000; ++i)
            d = (d + i) % 10007;
        printf(1, "d: %d\n", d);
    }
    else if (strcmp(argv[1], cmds[1]) == 0) {
        if (argc <= 3) exit();
        int pid = atoi(argv[2]);
        int priority = atoi(argv[3]);
        setPriority(pid, priority);
    }
    else if (strcmp(argv[1], cmds[2]) == 0) {
        printf(1, "pid: %d\n", getpid());
        printf(1, "level: %d\n", getLevel());
    }
    else if (strcmp(argv[1], cmds[3]) == 0) {
        __asm__("int $129");

        // a arbitrary task
        int d = 1;
        for (int i = 0; i < 90000000; ++i)
            d = (d + i) % 10007;
        printf(1, "d: %d\n", d);
    }
    else if (strcmp(argv[1], cmds[4]) == 0) {
        __asm__("int $130");
    }
    else {
        printf(1, "invalid command\n");
    }
    
    exit();
}