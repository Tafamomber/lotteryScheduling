#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

int
sys_getpinfo(void)
{
    struct pstat *p;
    if (argptr(0, (void*)&p, sizeof(*p)) < 0)
        return -1;
    
    acquire(&ptable.lock);
    for (int i = 0; i < NPROC; i++) {
        p->inuse[i] = ptable.proc[i].state != UNUSED;
        p->pid[i] = ptable.proc[i].pid;
        p->tickets[i] = ptable.proc[i].tickets;
        p->ticks[i] = ptable.proc[i].ticks;
    }
    release(&ptable.lock);
    return 0;
}

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
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

  argint(0, &pid);
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
sys_settickets(void) {
    int n;
    struct proc *p = myproc(); 

    argint(0, &n); 
    if (n < 0) { 
        return -1; 
    }

    p->tickets = n; 
    return 0; 
}
uint64 sys_getpinfo(void) {
    struct pstat *pstat; 
    struct proc *p;
    int i;

    
    argaddr(0, (uint64 *)&pstat);

    
    for (i = 0; i < NPROC; i++) {
        p = &proc[i];
        if (p->state == RUNNABLE || p->state == SLEEPING || p->state == RUNNING) {
            pstat[i].inuse = 1;
            pstat[i].pid = p->pid;
            pstat[i].tickets = p->tickets;
            
        } else {
            pstat[i].inuse = 0; 
        }
    }

    return 0; 
}
