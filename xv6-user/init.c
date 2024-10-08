// init: The initial user-level program

#include "kernel/include/types.h"
#include "kernel/include/stat.h"
#include "kernel/include/file.h"
#include "kernel/include/fcntl.h"
#include "xv6-user/user.h"
#include "kernel/include/sysnum.h"
//需要riscv下的这个函数 syscall0 ，以⽀持shutdown调⽤
#define __asm_syscall(...) \
    __asm__ __volatile__("ecall\n\t" \
                         : "=r"(a0) \
                         : __VA_ARGS__ \
                         : "memory"); \
    return a0;

static inline long __syscall0(long n)
{
    register long a7 __asm__("a7") = n;
    register long a0 __asm__("a0");
    __asm_syscall("r"(a7))
}

char *argv[] = {0};
// 这⾥放置了⽀持的系统调⽤
char *tests[] = {
    "brk",
    "chdir",
    "clone",
    "close",
    "dup",
    "dup2",
    "execve",
    "exit",
    "fork",
    "fstat",
    "getcwd",
    "getdents",
    "getpid",
    "getppid",
    "gettimeofday",
    "mkdir_",
    "mmap",
    "mount",
    "munmap",
    "openat",
    "open",
    "pipe",
    "read",
    "sleep",
    "test_echo",
    "times",
    "umount",
    "wait",
    "uname",
    "unlink",
    "write",
    "waitpid",
    "yield",
};

int counts = sizeof(tests) / sizeof((tests)[0]);

int main(void)
{
    int pid, wpid;
    dev(O_RDWR, CONSOLE, 0);
    dup(0); // stdout
    dup(0); // stderr

    for (int i = 0; i < counts; i++)
    {
        printf("init: starting sh\n");
        pid = fork();
        if (pid < 0)
        {
            printf("init: fork failed\n");
            exit(1);
        }
        if (pid == 0)
        {
            exec(tests[i], argv);
            printf("init: exec sh failed\n");
            exit(1);
        }

        for (;;)
        {
            wpid = wait((int *)0);
            if (wpid == pid)
            {
                break;
            }
            else if (wpid < 0)
            {
                printf("init: wait returned an error\n");
                exit(1);
            }
            else
            {
            }
        }
    }
    __syscall0(SYS_shutdown);
    return 0;
}
