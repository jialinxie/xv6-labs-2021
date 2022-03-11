#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
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


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
    uint64 startva, useradd;
  uint64 buf = 0;
  int num;
  pte_t* pte;
  struct proc *p = mycpu()->proc;

  if(argaddr(0, &startva)< 0)
    return -1;

  if(argint(1, &num) < 0)
    return -1;

  if(argaddr(2, &useradd) < 0)
    return -1;

  printf("startva = %p num = %d useradd = %p\n", startva, num, useradd);

  //It's okay to set an upper limit on the number of pages that can be scanned.
  if(startva > MAXVA){
    return -2;
  }

  for(int i = 0; i < num; i++){
    pte = walk(p->pagetable, startva, 0);
    if(*pte & PTE_A){
      buf |= (1L << i);
      *pte &= (~PTE_A);
    }

    startva += PGSIZE;
  }

  copyout(p->pagetable, useradd, (char *)&buf, sizeof(buf));
  return 0;
}
#endif

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