#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
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
  backtrace();

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

uint64 sys_sigalarm(void) {
  int ticks;
  uint64 handler;
  if (argint(0, &ticks) < 0) return -1;
  if (argaddr(1, &handler) < 0) return -1;
  struct proc *p = myproc();
  p->handler = handler;
  p->ticked = 0;
  p->ticks = ticks;
  return 0;
}

void restore_hframe(struct proc *p) {
  p->trapframe->ra = p->hf.ra;
  p->trapframe->sp = p->hf.sp;
  p->trapframe->gp = p->hf.gp;
  p->trapframe->tp = p->hf.tp;
  p->trapframe->t0 = p->hf.t0;
  p->trapframe->t1 = p->hf.t1;
  p->trapframe->t2 = p->hf.t2;
  p->trapframe->s0 = p->hf.s0;
  p->trapframe->s1 = p->hf.s1;
  p->trapframe->a0 = p->hf.a0;
  p->trapframe->a1 = p->hf.a1;
  p->trapframe->a2 = p->hf.a2;
  p->trapframe->a3 = p->hf.a3;
  p->trapframe->a4 = p->hf.a4;
  p->trapframe->a5 = p->hf.a5;
  p->trapframe->a6 = p->hf.a6;
  p->trapframe->a7 = p->hf.a7;
  p->trapframe->s2 = p->hf.s2;
  p->trapframe->s3 = p->hf.s3;
  p->trapframe->s4 = p->hf.s4;
  p->trapframe->s5 = p->hf.s5;
  p->trapframe->s6 = p->hf.s6;
  p->trapframe->s7 = p->hf.s7;
  p->trapframe->s8 = p->hf.s8;
  p->trapframe->s9 = p->hf.s9;
  p->trapframe->s10 = p->hf.s10;
  p->trapframe->s11 = p->hf.s11;
  p->trapframe->t3 = p->hf.t3;
  p->trapframe->t4 = p->hf.t4;
  p->trapframe->t5 = p->hf.t5;
  p->trapframe->t6 = p->hf.t6;

  p->trapframe->epc = p->hf.epc;
}
uint64 sys_sigreturn(void) {
  struct proc *p = myproc();
  restore_hframe(p);
  p->handler_running = 0;
  p->ticked = 0;
  return p->hf.a0;
}