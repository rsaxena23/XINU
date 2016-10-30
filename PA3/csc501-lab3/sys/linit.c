#include<conf.h>
#include<kernel.h>
#include<lock.h>
#include<sem.h>
#include<stdio.h>
#include<proc.h>

lock_map_t lock_tab[NLOCKS];

void linit()
{
int i;
for(i=0;i<NLOCKS;i++)
{
 lock_tab[i].lstate= LOCK_FREE;
 lock_tab[i].lock_opr= NO_OPR;
 lock_tab[i].active=0;
 lock_tab[i].start = (q_node *) getmem(sizeof(q_node));
 lock_tab[i].start->next=NULL;
 lock_tab[i].lock_id=i;

}

}

/*  Test Functions  */

void printLockTab()
{
STATWORD ps;
int i;
disable(ps);
kprintf("\nLock Tab State:%d\n",NLOCKS);
for(i=0;i<NLOCKS;i++)
{
  kprintf(" L%d: %d ",i,lock_tab[i].lstate);
}
enable(ps);

}


void printSemList()
{
int i;
  kprintf("\nSemaphore state:\n");
	for(i=0;i<NSEM;i++)
	{
	  kprintf("Sem %d: %d ",i,semaph[i]);
	}

}

