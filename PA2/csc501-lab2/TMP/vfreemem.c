/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{

	STATWORD ps;    
	struct	mblock	*p, *q;
	unsigned top;

	block = (struct mblock *) ((struct mblock *) block  -  (struct mblock *)(4096*NBPG)  + (struct mblock *) (BACKING_STORE_BASE  +   (proctab[currpid].store*BACKING_STORE_UNIT_SIZE)));

//	kprintf("\n Mem Loc: %x ",block);

	if (size==0 || (unsigned)block>(unsigned) (BACKING_STORE_BASE + (( proctab[currpid].store + 1 )*BACKING_STORE_UNIT_SIZE ))
	    || ((unsigned)block)<((unsigned) ( BACKING_STORE_BASE + ( proctab[currpid].store*BACKING_STORE_UNIT_SIZE)) ))
		{
//		kprintf("\n Free mem err: ");
		return(SYSERR);
		}
	size = (unsigned)roundmb(size);
	disable(ps);
	for( p=proctab[currpid].vmemlist.mnext,q= &proctab[currpid].vmemlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext )
		;
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &proctab[currpid].vmemlist) ||
	    (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
		restore(ps);
//		kprintf("\n Free mem error 2 ");
		return(SYSERR);
	}
	if ( q!= &proctab[currpid].vmemlist && top == (unsigned)block )
			q->mlen += size;
	else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}
	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}
	restore(ps);
//	kprintf("\n Free mem success");
	return(OK);



/*	kprintf("To be implemented!\n");
	return(OK); */
}
