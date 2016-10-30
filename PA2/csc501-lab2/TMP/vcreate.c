/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{

	int bsmid,new_pid;
	if(hsize>0 && hsize<=128)
	{	
	get_bsm(&bsmid);
//	kprintf("\n Private Heap BS: %d \n",bsmid);
	if(bsmid!=SYSERR)
	{
	new_pid=create(procaddr,ssize,priority,name,nargs,args);
	
	proctab[new_pid].store=bsmid;
	proctab[new_pid].vhpno= 4096;
	proctab[new_pid].vhpnpages= hsize;
	
	proctab[new_pid].vmemlist.mlen = 0;
	proctab[new_pid].vmemlist.mnext = (struct mblock *) (BACKING_STORE_BASE + (bsmid*BACKING_STORE_UNIT_SIZE));
	

	struct mblock *p =  BACKING_STORE_BASE + (bsmid*BACKING_STORE_UNIT_SIZE);
	p->mlen= hsize*NBPG;
	p->mnext= NULL; 
/*	proctab[new_pid].vmemlist->mnext = p;	*/
	

	shared_map *temp= (shared_map *) getmem(sizeof(shared_map));
	temp->bs_id=bsmid;
	temp->bs_vpno= 4096;
	temp->next = NULL;
	
	proctab[new_pid].next.next=temp;

	bsm_tab[bsmid].bs_status= BSM_MAPPED;
	bsm_tab[bsmid].bs_pid= new_pid;
	bsm_tab[bsmid].bs_vpno= 4096;
	bsm_tab[bsmid].bs_npages= hsize;
	bsm_tab[bsmid].bs_sem=1;
	
//	kprintf("\n Virtual process created ");
        return new_pid;
	}
	
	else
	{	
//	kprintf("\n System error: No more backing stores");
	
	}
		
	
	}
	
	//kprintf("To be implemented!\n");
	return SYSERR;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
