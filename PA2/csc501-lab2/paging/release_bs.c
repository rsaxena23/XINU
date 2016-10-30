#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

	int valid=0,counter=0;
  /* release the backing store with ID bs_id */

shared_map *temp = (proctab[currpid].next.next);
	shared_map *prev_temp;
        while(temp != NULL)
        {          
          if(temp->bs_id == bs_id)
                {
                valid = 1;
	/*	int j;
		for(j=5;j<1024;j++)
		{

		if( frm_tab[j].fr_pid==currpid  && frm_tab[j].fr_type==FR_TBL  )
		{
			frm_tab[j].fr_status=FRM_UNMAPPED;
			frm_tab[j].fr_type=FR_PAGE;
			frm_tab[j].fr_pid=-1;
			frm_tab[j].fr_vpno=-1;
			frm_tab[j].fr_refcnt=0;
		}

		} */


                break;
                }
        prev_temp = temp;
		temp = temp->next;		
		counter++;
        }
		if(valid==1)
		{
			bsm_tab[temp->bs_id].count--;
		
/*			temp->bs_id=-1;
			temp->bs_vpno=-1; */

			if(counter>1)
		    prev_temp ->next = temp ->next;

			else
			proctab[currpid].next.next=temp->next;

			temp->next=NULL;
			
			
			if(bsm_tab[temp->bs_id].count == 0)
			{
//			  kprintf("\nBS NPAGES GOING\n");
	
			  bsm_tab[temp->bs_id].bs_status = BSM_UNMAPPED;
			  bsm_tab[temp->bs_id].bs_pid = -1;
			  bsm_tab[temp->bs_id].bs_vpno= -1;
			  bsm_tab[temp->bs_id].bs_npages= -1;
			  bsm_tab[temp->bs_id].bs_sem = 0;
			}
			
			temp->bs_id=-1;
			temp->bs_vpno= -1;
			temp->next = NULL;
//			kprintf("\n\n BS %d released  cnt:%d  ",bs_id,bsm_tab[bs_id].count);
			return OK;
		}
		
		else
		{
		kprintf("\n BS hasnt been mapped, Unmap not possible ");
		return SYSERR;
		}


//   return OK;

}

