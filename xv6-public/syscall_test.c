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
        "intunlock",
        "longlock",
        "unlocktest",
        "yield"
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
    else if (strcmp(argv[1], cmds[5]) == 0) {
        __asm__("int $129");

        // a arbitrary task
        int d = 1;
        for (int i = 0; i < 990000000; ++i) 
            d = (d + i) % 10007;
        printf(1, "d: %d\n", d);
    }
    else if (strcmp(argv[1], cmds[6]) == 0) {
        __asm__("int $129");
        printf(1, "locked.\n");
        // a arbitrary task
        int d = 1;
        for (int i = 0; i < 990000000; ++i) {
            if (i == 30000000) {
                __asm__("int $130");
                printf(1, "unlocked.\n");
            }
            d = (d + i) % 10007;
        }
        printf(1, "d: %d\n", d);
        printf(1, "done.\n");
    }
    else if (strcmp(argv[1], cmds[7]) == 0) {
        yield();
    }
    else {
        printf(1, "invalid command\n");
    }
    
    exit();
}