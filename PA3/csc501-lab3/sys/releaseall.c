#include<conf.h>
#include<kernel.h>
#include<stdio.h>
#include<lock.h>
#include<proc.h>

int wakeUp(int lockno)
{
q_node *temp= lock_tab[lockno].start->next;
//kprintf("\n Printing Lock %d's list:",lockno);
int wcount=0,loopv_id;

if(temp==NULL)
 return EMPTY;

while(temp!=NULL)
{
  if(temp->operation==WRITE)
    break;

proctab[temp->pid].pwaitret=OK;
ready(temp->pid, RESCHNO);
loopv_id=temp->pid;
temp=temp->next;
//loopv_id=temp->pid;
deQ(lockno,loopv_id);
wcount++;

}

if(wcount==0)
{
proctab[temp->pid].pwaitret=OK;
ready(temp->pid, RESCHNO);
deQ(lockno,temp->pid);
}

return OK;

}

int releaseall(no_args,arg1)
int no_args;
long arg1;
{
STATWORD ps;
unsigned long *a;
int arr[no_args],call_resched=RESCHNO,op_status=OK;
disable(ps);

a = (unsigned long *)(&arg1) + (no_args-1); 
        for ( ; no_args > 0 ; no_args--)   
	{
	 arr[no_args-1] = *a--  ;
		arr[no_args - 1] = arr [ no_args - 1]% NLOCKS;
		
//	lock_tab[ arr[no_args-1] ].active--;
		if(procHasLock(arr[no_args - 1])==SYSERR)
		{		
		op_status=SYSERR;
		continue;
		} 
		 

          lock_tab[ arr[no_args-1] ].active--;
//	kprintf("\nReleased one lock of %d, active %d\n",arr[no_args - 1],lock_tab[ arr[no_args-1] ].active );

	if(lock_tab[ arr[no_args-1] ].active==0)
	{
	//kprintf("\n No more high reads/processes %d \n",arr[no_args - 1]);
//	printLockQ(arr[no_args-1]);
	lock_tab[arr[no_args-1]].lock_opr= NO_OPR;
 //	kprintf("\nop_status 1:%d",op_status);
        wakeUp(  arr[no_args-1]);       

	removePLock(arr[no_args - 1]); 
	  call_resched=RESCHYES;
	}
	}
if( call_resched ==  RESCHYES )
resched();

enable(ps);

//kprintf("\nop_status 2:%d",op_status);
return op_status;
}
