#include<conf.h>
#include<kernel.h>
#include<stdio.h>
#include<lock.h>
#include<proc.h>

int isInQ(int pid)
{
return OK;
}

void flushQ(int lockno)
{
	q_node *temp= lock_tab[lockno].start->next;
	//kprintf("\n Printing Lock %d's list:",lockno);
	int loopv_id;
	while(temp!=NULL)
	{
	//  kprintf("\nPID:%d  PRIO:%d  OPR:%d ",temp->pid,temp->prio,temp->operation);

	proctab[temp->pid].pwaitret=DELETED;
	ready(temp->pid, RESCHNO);
	loopv_id=temp->pid;
	temp=temp->next;
//	loopv_id=temp->pid;
	deQ(lockno,loopv_id);

//	temp=temp->next;
	}



 lock_tab[lockno].start->next=NULL;
 
}

int ldelete(int lockno)
{
//kprintf("\n LDEL:%d \n",currpid);
  STATWORD ps;
  disable(ps);

lockno=lockno%NLOCKS;

 if(lock_tab[lockno].lstate==LOCK_FREE)
 return SYSERR;
 
 lock_tab[lockno].lstate=LOCK_FREE;
 lock_tab[lockno].lock_opr=NO_OPR;
 //wakeUp(lockno);
 flushQ(lockno);
 resched();
restore(ps);
return OK;
}
