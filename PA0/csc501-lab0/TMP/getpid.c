/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
    if(systrack[currpid].proc_exc=='N')
systrack[currpid].proc_exc='Y';

systrack[currpid].call_count[GETPID]++;

	return(currpid);
}
