/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
	int valid = 0;

  /* sanity check ! */
//	kprintf("\n XM VP %d - start: %d     no_of_pages:%d",source,virtpage,npages);
  if ( (virtpage < 4096) || ( source < 0 ) || ( source > (MAX_ID-1)) ||(npages < 1) || ( npages >128)){
	kprintf("\n xmmap call error: parameter error! \n");
	return SYSERR;
  }
	
	  shared_map *temp = (proctab[currpid].next.next);
	while(temp != NULL)
	{
//		kprintf("\n bs:%d",temp->bs_id);
	  if(temp->bs_id == source)
		{
//		kprintf("\n Found BS: Mapping VPages ");
		valid = 1;
		break;
		}
	temp = temp->next;
	
	}	

	if(valid == 1)
	{

	if(  virtpage>=(temp->bs_vpno) &&  virtpage<( (temp->bs_vpno) + bsm_tab[temp->bs_id].bs_npages ) )
                {
  //                      kprintf("\n Virtual address already mapped to BS ");
                        return SYSERR;
                }
        if( npages> bsm_tab[temp->bs_id].bs_npages && bsm_tab[temp->bs_id].bs_npages>0)
                {
    //                    kprintf("\n Cannot Map Extra pages, Exp:%d  Req:%d",bsm_tab[temp->bs_id].bs_npages,npages);
                        return SYSERR;
                }

	 temp->bs_vpno = virtpage;

	 if(bsm_tab[source].count==1)
	{
		bsm_tab[source].bs_vpno= virtpage;
//		kprintf("\n First bsm tab[%d]  entre: XM-  %d    NPages:%d",source,bsm_tab[source].bs_vpno,bsm_tab[source].bs_npages);
	}

//	kprintf("\n Backing store mapped and virt page added: %d",temp->bs_vpno);
	return OK;
	}


  //	kprintf("\n Backing store not mapped! Didnt do GetBS!\n");
  	return SYSERR;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage )
{
	int valid =0,i;
  /* sanity check ! */

//	kprintf("\n Unmap called %d",virtpage);

  if ( (virtpage < 4096) ){ 
	kprintf("xmummap call error: virtpage (%d) invalid! \n", virtpage);
	return SYSERR;
  }

//	kprintf("\nUnmap started");

 	write_pages_to_bs(currpid);



  shared_map *temp = (proctab[currpid].next.next);
        while(temp != NULL)
        {
//		kprintf("\n UN:  BS: %d    VPNO: %d",temp->bs_id,temp->bs_vpno);	
          if(temp->bs_vpno == virtpage)
                {
                valid = 1;
                break;
                }
        temp = temp->next;
        }

        if(valid==1)
	{
/*		for(i=5;i<1024;i++)
	{
   if(frm_tab[i].fr_status==FRM_MAPPED && frm_tab[i].fr_pid==currpid && (frm_tab[i].fr_vpno>=virtpage  && (frm_tab[i].fr_vpno<=virtpage + bsm_tab[temp->bs_id].bs_npages) ))
        {
                frm_tab[i].fr_status = FRM_UNMAPPED;
                frm_tab[i].fr_pid = 0;
                frm_tab[i].fr_type = FR_PAGE;
                frm_tab[i].fr_loadtime = 0;
        }


	}	*/

	  temp->bs_vpno = -1;
//		kprintf("\n Unmap Successful %d :%d",temp->bs_id,currpid);
		if(bsm_tab[temp->bs_id].count==1)
		{
			bsm_tab[temp->bs_id].bs_vpno= -1;
		}
		
		return OK;
	}  

  kprintf("\n Not been mapped! unmap Not possible ");
  return SYSERR;
}
