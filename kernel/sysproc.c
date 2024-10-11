#include "include/types.h"
#include "include/riscv.h"
#include "include/param.h"
#include "include/memlayout.h"
#include "include/spinlock.h"
#include "include/proc.h"
#include "include/syscall.h"
#include "include/timer.h"
#include "include/kalloc.h"
#include "include/string.h"
#include "include/printf.h"
#include "include/sbi.h"
#include "include/vm.h"

#define SIGCHLD 17

extern int exec(char *path, char **argv);

uint64
sys_exec(void)
{
  char path[FAT32_MAX_PATH], *argv[MAXARG];
  int i;
  uint64 uargv, uarg;

  if(argstr(0, path, FAT32_MAX_PATH) < 0 || argaddr(1, &uargv) < 0){
    return -1;
  }
  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv)){
      goto bad;
    }
    if(fetchaddr(uargv+sizeof(uint64)*i, (uint64*)&uarg) < 0){
      goto bad;
    }
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    argv[i] = kalloc();
    if(argv[i] == 0)
      goto bad;
    if(fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  int ret = exec(path, argv);

  for(i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  return ret;

 bad:
  for(i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  return -1;
}

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0; 
}


uint64
sys_getpid(void)
{
  return myproc()->pid;
}



uint64
sys_fork(void)
{
  return fork();
}




uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}


uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int mask;
  if(argint(0, &mask) < 0) {
    return -1;
  }
  myproc()->tmask = mask;
  return 0;
}
uint64 
sys_shutdown(void) {
  sbi_shutdown();
  return 0;
}
uint64
sys_getppid(void){
  return myproc()->parent->pid;
}

// System call to get the current time of day
uint64 sys_gettimeofday(void) {
    uint64 addr;
    struct timespec tm;
    uint tick_counter;

    if (argaddr(0, &addr) < 0)
        return -1;

    tick_counter = r_time();
    tm.tv_sec = tick_counter / 1000000;
    tm.tv_nsec = (tick_counter % 1000000) * 1000;

    if (copyout2(addr, (char *)&tm, sizeof(tm)) < 0)
        return -1;

    return 0;
}

uint64
sys_uname(void){
  struct uname{
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
    char domainname[65];
  };
  struct uname uname={"xv6","xv6","0","0","xv6","localhost"};
  uint64 addr;
  argaddr(0,&addr);
  copyout2(addr,(char*)&uname,sizeof(uname));
  return 0;
}
// System call to get process times
uint64 sys_times(void) {
    uint64 addr;
    struct tms tm;
    uint tick_counter;

    if (argaddr(0, &addr) < 0)
        return -1;

    tick_counter = r_time();
    tm.tms_utime = tick_counter / 1000000;
    tm.tms_stime = tick_counter / 1000000;
    tm.tms_cutime = tick_counter / 1000000;
    tm.tms_cstime = tick_counter / 1000000;

    if (copyout2(addr, (char *)&tm, sizeof(tm)) < 0)
        return -1;

    return 0;
}

uint64
sys_brk(void)
{
  uint64 new_addr;
  int current_addr;
  int increment;

  if (argaddr(0, &new_addr) < 0)
    return -1;

  current_addr = myproc()->sz;

  if (new_addr == 0) {
    return current_addr;  
  }

  increment = (int)new_addr - current_addr;
  if (growproc(increment) < 0)
    return -1;

  return 0;
}

uint64
sys_nanosleep(void)
{
  uint64 addr;
  uint64 sleep_ticks;
  uint ticks0;

  if (argaddr(0, &addr) < 0)
    return -1;

  sleep_ticks = *(uint64 *)addr;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < sleep_ticks)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}




uint64
sys_yield(void){
  myproc() -> trapframe -> a0 = 0;
  yield();
  return 0;
}

uint64
sys_clone(void)
{
  int stack;
  if (argint(1, &stack) < 0)
    return -1;
  if (stack == 0)
  {
    return fork();
  }
  else
  {
    return clone(stack);
  }
}


uint64
sys_waitpid(void)
{
  int pid;
  uint64 code;
  if (argint(0, &pid) < 0)
    return -1;
  if (argaddr(1, &code) < 0)
    return -1;
  return wait4(pid, code);
}
