/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <lab0.h>
#include <proc.h>
extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{

if(systrack[currpid].proc_exc=='N')
systrack[currpid].proc_exc='Y';

systrack[currpid].call_count[GETTIME]++;

    /* long	now; */

	/* FIXME -- no getutim */

    return OK;
}
