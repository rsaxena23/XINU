#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

extern int currpid;
extern int numproc;
extern struct pentry proctab[];
static unsigned long *esp;

void printprocstks(int priority)
{
int i;
kprintf("\n\nProcesses with lower priority:\n");
for(i=0;i<NPROC;i++)
{
   if(proctab[i].pstate==PRFREE)
   continue;
   
  if(proctab[i].pprio<=priority)
  {
    kprintf("\nPID:%d  Proc_name:%s   Proc_Prio:%d",i,proctab[i].pname,proctab[i].pprio);
	
	if(i==currpid)
	{
	asm("movl %esp,esp");
	kprintf("\nStack pointer:%x",esp);
	}
	else
	kprintf("\nStack pointer:%x",proctab[i].pesp);
	
	kprintf("  Stack_Limit:%x  Stack_Size:%d  Stack_Base:%x",proctab[i].plimit,proctab[i].pstklen,proctab[i].pbase);
  }
}
}
