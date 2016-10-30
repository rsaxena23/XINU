/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */


SYSCALL pfint()
{


	int valid = 0; 
	unsigned int fault_addr= read_cr2();
	pd_t *ptr;
	pt_t *ptr2;
	
	int pd_offset,pt_offset, virtpagenum;
	virtpagenum= (fault_addr>>12);
	pd_offset = (fault_addr>>22);
	ptr = (unsigned)proctab[currpid].pdbr + (unsigned)(pd_offset*sizeof(int));
	pt_offset = ( ((fault_addr>>12))&(0x000003ff)  );

//	check_frm_status();

/*	if(grpolicy()==LRU)
		updateAccessTab();*/
	
//	kprintf("\n Page Fault Handler:  PD_off:%d  PT_off:%d  VirtPage:%d  ",pd_offset,pt_offset,virtpagenum);
//	kprintf("\n PDBR: %x     pointer:%x",proctab[currpid].pdbr,ptr);
	
	if(ptr->pd_pres==0)
	{
	//page directory missing
//		kprintf(" \npd missing");

	shared_map *temp = proctab[currpid].next.next;
	while(temp!=NULL)
	{
//		kprintf(" vpno:%d   id:%d    %d",temp->bs_vpno,temp->bs_id,bsm_tab[temp->bs_id].bs_npages);
	  
		  if(virtpagenum>=temp->bs_vpno && ( (temp->bs_vpno)+ bsm_tab[temp->bs_id].bs_npages)>virtpagenum )
		{
		  valid=1;
		  break;
		}
	    temp=temp->next;
	} 

	if(valid==1)
	{
	int i,a;
	ptr -> pd_pres= 1;	
//	check_frm_status();
	get_frm(&a);
//	check_frm_status();
	ptr->pd_base = (a>>12);
//	kprintf("\n \n :::%d    ptr: %x ",(a>>12),ptr);
	ptr2  = (pt_t *) ((ptr->pd_base)*NBPG);
//	kprintf("\n  base: %d    ptr2:  %x ",ptr->pd_base,ptr2);

	for(i=0;i<1024;i++)
	{
//		*ptr2 = {0,1,0,0,0,0,0,0,0,0,0};
		ptr2->pt_pres=0;
		ptr2->pt_write=1;
		ptr2->pt_user=0;
		ptr2->pt_base=23; 

		ptr2++;
	} 
//	kprintf("\n base: %d  %d",ptr->pd_base,(a>>12));


	frm_tab[(a>>12) - FRAME0].fr_type= FR_TBL;
//	frm_tab[(a>>12) - FRAME0].fr_vpno = virtpagenum;
	
//	kprintf("\n base: %d   pres:%d   ",ptr->pd_base,ptr->pd_pres);
	
	return OK;
	}
	else
	{
	  
//	kprintf("\n Fault Pages Not mapped: PD entry not poss! ");
	
	return SYSERR;
	}
	
	}
	
	else
	{
	ptr2 = ((ptr->pd_base) * 4096) +  ( pt_offset * sizeof(int)  );    
	
	if(ptr2->pt_pres ==0)
	{

	// page table missing
//	kprintf("\n ptmissing");

	shared_map *temp = proctab[currpid].next.next;
	while(temp!=NULL)
	{
	    if(virtpagenum>=temp->bs_vpno && ((temp->bs_vpno)+ bsm_tab[temp->bs_id].bs_npages)>virtpagenum )
		{
		//	kprintf("\n ");
		  valid=1;
		  break;
		}
	    temp=temp->next;
	} 
	if(valid==1)
	{	
	int b;
	ptr2->pt_pres= 1;
	get_frm(&b);	
	frm_tab[ (b>>12) - FRAME0].fr_vpno= virtpagenum;
//	kprintf("\n Page Added-   VPN:%d     FRAME:%d    BaseVP:%d        BID:%d  ",virtpagenum,((b>>12)-FRAME0),temp->bs_vpno,temp->bs_id);
	
	read_bs((char *)b,temp->bs_id,virtpagenum - (temp->bs_vpno));
	ptr2->pt_base=(b>>12);

	
	return OK;
	}
	
	else
	{
//	kprintf("\n Fault Pages Not mapped: PT entry not poss! ");
	
	return SYSERR;
	}
	
	}
	
/*	else
	{
//	kprintf("\n Whats happening! PD and PT there");
	} */
	}
	


//  kprintf("To be implemented! Interrupt Handler!\n");
//  return OK;
}


