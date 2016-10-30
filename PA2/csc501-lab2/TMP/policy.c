/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>


extern int page_replace_policy;
/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
int fifo_counter=0;
int lru_counter = 0;
SYSCALL srpolicy(int policy)
{
  /* sanity check ! */
	page_replace_policy=policy;

	if(policy==FIFO)
	fifo_counter =0;
		
	else if(policy==LRU)
	lru_counter = 0;

//  kprintf("To be implemented!\n");

  return OK;
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy()
{
  return page_replace_policy;
}
