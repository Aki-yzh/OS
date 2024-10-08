#ifndef __SYSNUM_H
#define __SYSNUM_H

// System call numbers
#define SYS_fork        1
#define SYS_clone       220
#define SYS_exit        93
#define SYS_wait        300
#define SYS_pipe        59
#define SYS_read        63
#define SYS_kill        6
#define SYS_exec        221
#define SYS_fstat       80
#define SYS_chdir       49
#define SYS_dup         23
#define SYS_dup3        24
#define SYS_getpid      172
#define SYS_sbrk        200
#define SYS_brk         214
#define SYS_sleep       450
#define SYS_uptime      169
#define SYS_open        270
#define SYS_openat      56
#define SYS_write       64
#define SYS_remove      400
#define SYS_trace       18
#define SYS_sysinfo     160
#define SYS_mkdir       33
#define SYS_mkdirat     34
#define SYS_close       57
#define SYS_test_proc   22
#define SYS_dev         10
#define SYS_readdir     27
#define SYS_getcwd      17
#define SYS_rename      26
#define SYS_shutdown    210
#define SYS_wait4       260 
#define SYS_sched_yield 124
#define SYS_getppid     173
#define SYS_unlinkat    35
#define SYS_gettimeofday 169
#define SYS_nanosleep   101
#define SYS_times       153
#define SYS_getdents64  61
#define SYS_munmap      215
#define SYS_mmap        222
#define SYS_linkat      37
#define SYS_umount2     39
#define SYS_mount       40

#endif