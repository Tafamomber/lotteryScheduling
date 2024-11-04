#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "pstat.h"  

// Simple random number generator (if not already available in xv6)
static unsigned long next = 1;
int random(void) {
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

struct {
    struct spinlock lock;
    struct proc proc[NPROC];
} ptable;

struct pstat ptable[NPROC];  

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

// Initialize process table lock
void
pinit(void)
{
    initlock(&ptable.lock, "ptable");
}

// Allocate a new process
static struct proc*
allocproc(void)
{
    struct proc *p;
    char *sp;

    acquire(&ptable.lock);

    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
        if(p->state == UNUSED)
            goto found;

    release(&ptable.lock);
    return 0;

found:
    p->state = EMBRYO;
    p->pid = nextpid++;
    p->tickets = 1;  // Initialize tickets with default value of 1

    // Initialize pstat entry
    ptable[p->pid % NPROC].inuse = 1;
    ptable[p->pid % NPROC].pid = p->pid;
    ptable[p->pid % NPROC].tickets = p->tickets;
    ptable[p->pid % NPROC].priority = 0;
    ptable[p->pid % NPROC].ticks = 0;

    release(&ptable.lock);

    // Allocate kernel stack.
    if((p->kstack = kalloc()) == 0){
        p->state = UNUSED;
        return 0;
    }
    sp = p->kstack + KSTACKSIZE;

    // trap frame.
    sp -= sizeof *p->tf;
    p->tf = (struct trapframe*)sp;

    
    sp -= 4;
    *(uint*)sp = (uint)trapret;

    sp -= sizeof *p->context;
    p->context = (struct context*)sp;
    memset(p->context, 0, sizeof *p->context);
    p->context->eip = (uint)forkret;

    return p;
}


void
scheduler(void)
{
    struct proc *p;
    for(;;){
        //interrupts on this processor.
        sti();

        acquire(&ptable.lock);

        // 1. Calculate the total number of tickets among runnable processes
        int total_tickets = 0;
        for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
            if(p->state == RUNNABLE) {
                total_tickets += p->tickets;
            }
        }

        if(total_tickets == 0) {
            release(&ptable.lock);
            continue; 
        }

        // 2. Generate a random winning ticket
        int winning_ticket = random() % total_tickets;

        // 3. Find the process corresponding to the winning ticket
        int current_ticket = 0;
        for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
            if(p->state == RUNNABLE) {
                current_ticket += p->tickets;
                if(current_ticket > winning_ticket) {
                    // Found the winning process
                    p->state = RUNNING;

                    // Update pstat ticks for this process
                    ptable[p->pid % NPROC].ticks++;

                    swtch(&cpu->scheduler, p->context);

                    
                    p->state = RUNNABLE;
                    break;
                }
            }
        }

        release(&ptable.lock);
    }
}
