#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>



int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
int i;	
	STATWORD ps;
	disable(ps);
		
//	kprintf("\n GETBS by %d \n",currpid);
	
	if(bsm_tab[bs_id].bs_sem==1 || npages<=0 || npages>128)
	{
//	kprintf("\n\n Illegal! \n");
	return SYSERR;
	}
	
	else if(bsm_tab[bs_id].bs_status==BSM_MAPPED)
	{	
	npages = bsm_tab[bs_id].bs_npages;	
//	kprintf("\n\n BSM %d  already set with Pages:%d   cnt:%d     pid:%d\n\n",bs_id,npages,bsm_tab[bs_id].count,bsm_tab[bs_id].bs_pid);
	}

	else
	{
	bsm_tab[bs_id].bs_status= BSM_MAPPED;
	bsm_tab[bs_id].bs_pid= currpid;	
	bsm_tab[bs_id].bs_npages= npages;
	bsm_tab[bs_id].bs_sem=0;	
//	kprintf("\n\n BSM %d  set first time Pages:%d   cnt:%d \n\n",bs_id,npages,bsm_tab[bs_id].count);
	}
	
	// Start of bs mapping in proctab

  
	shared_map *trav =(shared_map *) proctab[currpid].next.next;
    	shared_map *prev_trav;
//	kprintf("\n Before insert : ");
	while(trav != NULL)
		{
		prev_trav=trav;
//		kprintf("\n  BS: %d   VP: %d",trav->bs_id,trav->bs_vpno);
                trav=  trav-> next;
		}

        shared_map *temp= (shared_map *)  getmem(sizeof(shared_map));

        temp->bs_id = bs_id;
        temp->bs_vpno = -1;
        temp->next= NULL;

//	kprintf("\n proctab next before: %d     temp:%x ",proctab[currpid].next,&temp);
	
	if(proctab[currpid].next.next==NULL)
	{
        proctab[currpid].next.next = temp;
	shared_map *tp = proctab[currpid].next.next;
//	kprintf("\n First print of BS map:   ID: %d    VPNO: %d    N: %d",tp->bs_id,tp->bs_vpno,tp->next);
	}

	else  
	prev_trav->next =  temp;

//	kprintf("\n proctab next before: %d",proctab[currpid].next);

//	kprintf("\nTemp:  BS:%d  VPNO:%d",temp->bs_id,temp->bs_vpno);	

/*	shared_map *temp2 = proctab[currpid].next.next;

	while(temp2!=NULL)
	{
//		kprintf("\n Values BSID:%d    VPNO:%d",temp2->bs_id,temp2->bs_vpno);
	    temp2=temp2->next;
	}
	*/
	// End of bs mapping in proctab
	
        bsm_tab[bs_id].count++;

//	kprintf("\n Backing store got! %d ", bs_id);
	restore(ps);
    return npages;

}


