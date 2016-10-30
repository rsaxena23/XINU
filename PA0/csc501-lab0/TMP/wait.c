/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL	wait(int sem)
{
	STATWORD ps;    
	struct	sentry	*sptr;
	struct	pentry	*pptr;

if(systrack[currpid].proc_exc=='N')
systrack[currpid].proc_exc='Y';

systrack[currpid].call_count[WAIT]++;


	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		return(SYSERR);
	}
	
	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		enqueue(currpid,sptr->sqtail);
		pptr->pwaitret = OK;
		resched();
		restore(ps);
		return pptr->pwaitret;
	}
	restore(ps);
	return(OK);
}
