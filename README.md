# lotteryScheduling

Modifications Summary
1. proc.c
Function Added: settickets()

This function allows processes to set their ticket count for scheduling.
Changes to allocproc() Function:

Added p->tickets = 1; to initialize the ticket count for newly allocated processes.
Modifications in scheduler() Function:

Implemented logic to select a process based on its ticket count. The scheduler now randomly selects a process to run according to its tickets.
2. sysproc.c
System Call Added: sys_getpinfo()
This system call retrieves information about the process, including the number of tickets assigned.
3. syscall.c
Declarations Added:
Added extern uint64 sys_getpinfo(void); to declare the new system call.
Syscall Table Updated:
Registered the new system call in the syscall table with sys_getpinfo.
4. defs.h
New Definition Added:
Defined a new syscall number SYS_getpinfo to identify the new system call.
5. user.h
Function Prototype Added:
Added a prototype for getpinfo() in the user-level header file to allow user programs to call this system call.
