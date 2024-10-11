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
  "exit",  //1/2
  "getpid",   //done
  "getppid",     //done    
  "gettimeofday", //done
  "uname",     //done    
  "times",    //done
  "brk",    //done
  "clone",
  "fork",   //2/3
  "wait",
  "waitpid",
  "mmap",    //2/3
  "munmap",   //2/4
  "execve",   //done
  "close",    //done
  "dup2",
  "dup",     //done       
  "pipe",
  "read",   //done
  "write",    //done      
  "openat",  //done
  "open",   //done
  "fstat",   //done
  "getdents",
  "chdir",   //done
  "getcwd",  //done
  "mkdir",  //done
  "unlink",
  "mount",
  "unmount",
  "mkdir_",  //done
  "sleep",   //done
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
