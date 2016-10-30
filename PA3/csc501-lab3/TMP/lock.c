#include<conf.h>
#include<kernel.h>
#include<stdio.h>
#include<lock.h>
#include<proc.h>

unsigned long ctr1000;

void enQ(int lockno, int type,int priority)
{
  

 q_node *temp= lock_tab[lockno].start;
 q_node *prev_temp=temp;
 temp=temp->next;

//kprintf("\n %d About to be enQ'd into %d   Type: %d   temp:%d \n",currpid,lockno,type,priority,temp);

 while(temp!=NULL)
 {
//	kprintf("\nPID:%d  PRIO:%d  OPR:%d ",temp->pid,temp->prio,temp->operation);
    if(priority>temp->prio || ( type==WRITE &&  (priority==(temp->prio)) && ( (ctr1000 - (temp->waitTime)) <1000 ))  )
	{
	  q_node *node_cr = (q_node *) getmem(sizeof(q_node));
	  node_cr->pid=currpid;
	  node_cr->prio=priority;
	  node_cr->operation=type;
	  node_cr->waitTime= ctr1000;
	  node_cr->next=temp;
	  prev_temp->next=node_cr;
	  break;
	}
	prev_temp=temp;
	temp=temp->next;
 }
 
 if(temp==NULL)
 {
 q_node *node_cr = (q_node *) getmem(sizeof(q_node));
	  node_cr->pid=currpid;
	  node_cr->prio=priority;
	  node_cr->operation=type;
	  node_cr->next=NULL;
	  prev_temp->next= node_cr;
//  kprintf("\n Inserted node_cr:%d  temp:%d   prev_temp:%d   start: %d  ",node_cr,temp,prev_temp,lock_tab[lockno].start->next);
   
 }
//kprintf("\n Lock %d Wait:%d ",lockno,currpid);

}

void printLockQ(int lockno)
{
q_node *temp= lock_tab[lockno].start->next;
kprintf("\n Printing Lock %d's list:",lockno);
while(temp!=NULL)
{
  kprintf("\nPID:%d  PRIO:%d  OPR:%d ",temp->pid,temp->prio,temp->operation);
temp=temp->next;
}
}


void deQ(int lockno,int pid)
{

q_node *temp= lock_tab[lockno].start->next;
q_node *prev_temp=lock_tab[lockno].start;
//kprintf("\n Printing Lock %d's list:",lockno);
//kprintf("\n Lock %d  DEQin %d\n",lockno,pid);
while(temp!=NULL)
{
  if(temp->pid==pid)
	{
	 prev_temp->next=temp->next;
	break;
	
	}
prev_temp=temp;
temp=temp->next;
}
//kprintf("\n Printing after deQ ");
//printLockQ(lockno);
 
}

/*
void printLockQ(int lockno)
{
q_node *temp= lock_tab[lockno].start->next;
kprintf("\n Printing Lock %d's list:",lockno);
while(temp!=NULL)
{
  kprintf("\nPID:%d  PRIO:%d  OPR:%d ",temp->pid,temp->prio,temp->operation);
temp=temp->next;
}
}
*/

/*### proc lock list area### */
void addPLock(int lockno)
{

plock_node *temp= proctab[currpid].start->next;
	plock_node *prev_temp= proctab[currpid].start;
//	kprintf("\nADD:\n");
	while(temp!=NULL)
	{
	//	  kprintf("\nPID:%d LOCKID:%d  ",currpid,temp->lockno);
	prev_temp=temp;
	temp=temp->next;
	}
        plock_node *new_node = (plock_node *) getmem(sizeof(plock_node));
	new_node->lockno = lockno;
	new_node->next = NULL;
	prev_temp->next= new_node;
	

}
void printPList(int pid)
{
  plock_node *temp= proctab[pid].start->next;
	kprintf("\n PID:%d Lock List:\n",pid);
	 while(temp!=NULL)
        {
                kprintf("\nLOCKID:%d  ",temp->lockno);
	temp=temp->next;
	}
}

int removePLock(int lockno)
{
 plock_node *temp= proctab[currpid].start->next;
        plock_node *prev_temp= proctab[currpid].start;
//	kprintf("\nRemove:\n");
        while(temp!=NULL)
        {
//		kprintf("\nPID:%d LOCKID:%d  ",currpid,temp->lockno);
               if(temp->lockno==lockno)
		{
		prev_temp->next=temp->next;
		return OK;
		}
        prev_temp=temp;
        temp=temp->next;
        }
  return SYSERR;
}

int procHasLock(int lockno)
{
  plock_node *temp= proctab[currpid].start->next;
        plock_node *prev_temp= proctab[currpid].start;
        while(temp!=NULL)
        {
		if(temp->lockno==lockno)                           
                return OK;                

        prev_temp=temp;
        temp=temp->next;
        }
//kprintf("\n Not Found\n");
return SYSERR;
}

/*########################## */


int lock(int ldes1, int type, int priority)
{
   STATWORD ps;
   struct pentry *pptr;
   disable(ps);
	if(ldes1!=lock_tab[ldes1%NLOCKS].lock_id)
	{
	restore(ps);
        return SYSERR;
	}
 ldes1=ldes1%NLOCKS;

//	pptr = &proctab[currpid];
  /*
  if lock state free  --> acquire
  
  if lock opr is read, type is read, and no write with higher priority -->acquire
  
  if lock opr is write and lock state not free --> wait
  
  else wait
  
  
  */
/*	if(pptr->pwaitret==DELETED)
	return SYSERR;*/

	addPLock(ldes1);

   if(lock_tab[ldes1].lock_opr==NO_OPR)
	{
      lock_tab[ldes1].active++;
	lock_tab[ldes1].lock_opr=type;
   restore(ps);
   return OK;
	}
   
   else
   {
	if(lock_tab[ldes1].lock_opr==READ && type==READ)
	{
	  //check if Write present with higher

		if( higherThanWrite(ldes1,priority)==TRUE )
		{
		lock_tab[ldes1].active++;		
		restore(ps);
		return OK;
		}

		else
		{
		pptr = &proctab[currpid];
		pptr->pstate = PRWAIT;
		enQ(ldes1,type,priority);
//		printLockQ(ldes1);
		pptr->pwaitret = OK;
		resched();
		restore(ps);
//		kprintf("\n Currpid:%d  Return from Wait %d \n",currpid,pptr->pwaitret);
	if(pptr->pwaitret==OK)
	{
	lock_tab[ldes1].active++;	
	lock_tab[ldes1].lock_opr= type;
	}

	return pptr->pwaitret;
		}
	}
	else
	{
		pptr = &proctab[currpid];
		pptr->pstate = PRWAIT;
	    enQ(ldes1,type,priority);
//		printLockQ(ldes1);
		pptr->pwaitret = OK;
                resched();
                restore(ps);
	//	kprintf("\n Currpid:%d  Return from Wait %d \n",currpid,pptr->pwaitret);
		
	if(pptr->pwaitret==OK)
	{
        lock_tab[ldes1].active++;
	lock_tab[ldes1].lock_opr= type;
	}

		return pptr->pwaitret;

	}
   
   }
 restore(ps);
   return SYSERR;

}

 int higherThanWrite(int lockno,int priority)
{
 
	q_node *temp= lock_tab[lockno].start->next;
	//kprintf("\n Printing Lock %d's list:",lockno);
	
	while(temp!=NULL)
	{
  		if(temp->prio>priority && temp->operation==WRITE)
		return FALSE;

		temp=temp->next;
	}

	return TRUE;
}
