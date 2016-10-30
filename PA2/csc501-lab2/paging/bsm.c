/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>




/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */

bs_map_t bsm_tab[MAX_ID];

SYSCALL init_bsm()
{

int i;

//kprintf("\n BSM start  \n");
//    check_frm_status();
//kprintf("\n BSM addr: %d   size: %d  \n FRM addr: %d   size:  %d \n",&(bsm_tab[0]),sizeof(bsm_tab[0])*16,&(frm_tab[0]),sizeof(frm_tab[0])*1024);


for(i=0;i<MAX_ID;i++)
{
 bsm_tab[i].bs_status=BSM_UNMAPPED;                     /* MAPPED or UNMAPPED           */
 bsm_tab[i].bs_pid= -1;                          /* process id using this slot   */
 bsm_tab[i].bs_vpno=-1;                         /* starting virtual page number */
 bsm_tab[i].bs_npages=-1;                       /* number of pages in the store */
 bsm_tab[i].bs_sem=0;                           /* semaphore mechanism ?        */
 bsm_tab[i].count = 0;
}


//kprintf("\n BSM End \n");
//check_frm_status();

}

void check_bs_status()
{

int i;

for(i=0;i<16;i++)
        {
                kprintf(" i: %d ",bsm_tab[i].bs_status);
        }

}


/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	
	int i;
	for(i=0;i<MAX_ID;i++)
	{
		if(bsm_tab[i].bs_status==BSM_UNMAPPED)
		break;
	}
	if(i!=MAX_ID)	
	 *avail = i;	 
	
	else
	*avail = SYSERR;

//	kprintf("\n BSM: %d ",i);
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
if(i<MAX_ID && i>=0)
{
bsm_tab[i].bs_status= BSM_UNMAPPED;
bsm_tab[i].bs_pid= -1;
bsm_tab[i].bs_vpno= -1;
bsm_tab[i].bs_npages= 0;
bsm_tab[i].bs_sem=0;
}
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
}

SYSCALL write_pages_to_bs(int pid)
{
int i,count=0;
pd_t *ptr1;
pt_t *ptr2;

for(i=5;i<1024;i++)
{
  if(frm_tab[i].fr_pid==pid && frm_tab[i].fr_type==FR_PAGE && frm_tab[i].fr_vpno>=4096)
  {
	ptr1 = proctab[pid].pdbr +  ( (frm_tab[i].fr_vpno) >> 10) * sizeof(int);
		ptr2 =  (ptr1->pd_base)*NBPG   +    ((frm_tab[i].fr_vpno)&(0x000003ff))*sizeof(int);
	
	if(ptr2->pt_dirty=1)
	{		
	//	kprintf("\n FRID: %d: %d",i,frm_tab[i].fr_vpno);
//	kprintf("\n Found Dirty page \n");
	shared_map *temp= proctab[pid].next.next;
	while(temp!=NULL)
	{
	//		kprintf("\n Writepid:%d     BS:%d    VPN_Base:%d\n ",pid,temp->bs_id,temp->bs_vpno);
	     if((temp->bs_vpno)<=frm_tab[i].fr_vpno  && frm_tab[i].fr_vpno<( (temp->bs_vpno) + bsm_tab[temp->bs_id].bs_npages  ) )
		 {
		
	//	kprintf("\nWrite Currpid:%d    BS:%d     PG:%d     VPN:%d     frame:%d",pid,temp->bs_id,frm_tab[i].fr_vpno - (temp->bs_vpno),frm_tab[i].fr_vpno,(FRAME0 + i));
		 write_bs( (i+FRAME0)*NBPG, temp->bs_id , frm_tab[i].fr_vpno - (temp->bs_vpno)   );

	/*	get_frm(&a);
	 read_bs(a, temp->bs_id , frm_tab[i].fr_vpno - (temp->bs_vpno));
		int *ptr = a;
		kprintf("\nValue:%c ",(char)*ptr); */

		 break;
		 }
		 temp=temp->next;
	}
	
	}
  
  }
}

return OK;
}

SYSCALL read_pages_from_bs(int pid)
{

int i;
for(i=5;i<1024;i++)
{
  if(frm_tab[i].fr_pid==pid && frm_tab[i].fr_type==FR_PAGE)
  {
	if(frm_tab[i].fr_vpno>=4096)
		{
			shared_map *temp= proctab[pid].next.next;
			while(temp!=NULL)
			{
					
			if((temp->bs_vpno)<=frm_tab[i].fr_vpno  && frm_tab[i].fr_vpno<( (temp->bs_vpno) + bsm_tab[temp->bs_id].bs_npages  ) )
			{
//			kprintf("\n Page read ");
	//		kprintf("\nRead Currpid:%d    BS:%d     PG:%d     VPN:%d",pid,temp->bs_id,frm_tab[i].fr_vpno - (temp->bs_vpno),frm_tab[i].fr_vpno);
			read_bs( (i+FRAME0)*NBPG, temp->bs_id , frm_tab[i].fr_vpno - (temp->bs_vpno)   );
				break;
			}
			temp=temp->next;
			}
		}
	}
}

return OK;
}

void printbs(int bsid,int n)
{
  int i;
 char *a = BACKING_STORE_BASE + (bsid*BACKING_STORE_UNIT_SIZE) ;
kprintf("\n BS %d :",bsid);
   for(i=0;i<n;i++)
{
kprintf(" %d: %c ",(i),*a);
a+= NBPG;
}

}


void updateAccessTab()
{
 int i;
	pd_t *ptr1;
	pt_t *ptr2;

	lru_counter ++;

  for(i=5;i<1024;i++)
	{

	if(frm_tab[i].fr_type==FR_PAGE && frm_tab[i].fr_status==FRM_MAPPED)
	{
	ptr1 = proctab[frm_tab[i].fr_pid].pdbr +  ( ( (frm_tab[i].fr_vpno) >> 10 )   * sizeof(int) );
                ptr2 =  (ptr1->pd_base)*NBPG   +   ( (  (frm_tab[i].fr_vpno)&(0x000003ff))*sizeof(int));
	if(ptr2 ->pt_acc==1)
	{
	//	kprintf("\n Acc set:%d   frno:%d",lru_counter,i);
		frm_tab[i].fr_loadtime=lru_counter;
		ptr2->pt_acc=0;
		
	}
		
			
	}	
	}
}

void unmap_all(int pid)
{
 shared_map *temp= proctab[pid].next.next;

	while(temp!=NULL)
	{
//		kprintf("\n Unmapping ALL: BS:%d   VP:%d",temp->bs_id,temp->bs_vpno);
	
		if(temp->bs_id>=0 && temp->bs_id<MAX_ID)
		{	
			bsm_tab[temp->bs_id].count--;
			if(bsm_tab[temp->bs_id].count==0)
			{
			bsm_tab[temp->bs_id].bs_status= BSM_UNMAPPED;
			bsm_tab[temp->bs_id].bs_pid = -1;
			bsm_tab[temp->bs_id].bs_vpno = -1;
			
			}
			temp->bs_id=-1;
			temp->bs_vpno=-1;
		}
		temp=temp->next;
	}
	proctab[pid].next.next=NULL;


}
