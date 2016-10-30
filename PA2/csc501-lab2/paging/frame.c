/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
#include <sleep.h>
/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */

fr_map_t frm_tab[1024];

void first_ptentry(int);

void check_frm_status()
{
int i;
kprintf("\n FRM ");
for(i=5;i<10;i++)
{
   kprintf("\n %d:   PID:%d  TYPE:%d",i,frm_tab[i].fr_pid,frm_tab[i].fr_type);
}

}

int min_frm_status()
{
int i,min = 0x0fffffff,fr=-1;
//kprintf("\n FRM ");

if(grpolicy()==FIFO)
{
for(i=5;i<1024;i++)
{
//	kprintf("\n %d  ",frm_tab[i].fr_pid);
   if(frm_tab[i].fr_refcnt<min && frm_tab[i].fr_type==FR_PAGE)
	{
	    min=frm_tab[i].fr_refcnt;
		fr=i;	
	}
}

}


else if(grpolicy()==LRU)
{

for(i=5;i<1024;i++)
{
  if(frm_tab[i].fr_type==FR_PAGE && frm_tab[i].fr_status==FRM_MAPPED)
	{
	fr=i;
	min = frm_tab[i].fr_loadtime;
	break;
	}
}


for(i=fr;i<1024;i++)
{
 //     kprintf("   FR%d:%d   ",(FRAME0 + i),frm_tab[i].fr_loadtime);
   if(frm_tab[i].fr_loadtime<=min && frm_tab[i].fr_type==FR_PAGE)
        {
		if(frm_tab[i].fr_loadtime==min && frm_tab[i].fr_vpno<frm_tab[fr].fr_vpno)
		continue;
	
            min=frm_tab[i].fr_loadtime;
                fr=i; 
	//	kprintf("\n  Min: %d,  frame no: %d",min,fr);
       }
}


}
return fr;

}


void flush_pages(int pid)
{
int i;

for(i=5;i<1024;i++)
{

   if(frm_tab[i].fr_status==FRM_MAPPED && frm_tab[i].fr_pid==pid)
	{
	//	kprintf("\nFrame freed %d",(FRAME0 + i));
		frm_tab[i].fr_status= FRM_UNMAPPED;
		frm_tab[i].fr_vpno=-1;
		frm_tab[i].fr_pid= -1;
		frm_tab[i].fr_type=FR_PAGE;
		frm_tab[i].fr_loadtime=0;
	}
}

}


SYSCALL init_frm()
{

	int i,j;

	pt_t *pt_ptr = (pt_t *) (FRAME0*NBPG);
		
	for(i=0;i<4;i++)
	{
				
		frm_tab[i].fr_status= FRM_MAPPED;
		frm_tab[i].fr_pid=0;				
		frm_tab[i].fr_vpno=i*1024;				
		frm_tab[i].fr_refcnt=0;			
		frm_tab[i].fr_type=FR_TBL;				
		frm_tab[i].fr_dirty=0;
		frm_tab[i].cookie=NULL;				
		frm_tab[i].fr_loadtime=0;
		
		for(j=0;j<1024;j++)
		{
			pt_t temp= {1,1,0,0,0,0,0,0,0,0, ((i*1024) + j)};
			*pt_ptr= temp;
			pt_ptr++;
			
		}
	}	
	
	pd_t *pd_ptr = (pd_t *) ((FRAME0 + 4)*NBPG);	
	
		frm_tab[i].fr_status= FRM_MAPPED;
		frm_tab[i].fr_pid=0;				
		frm_tab[i].fr_vpno=0;				
		frm_tab[i].fr_refcnt=0;			
		frm_tab[i].fr_type=FR_DIR;				
		frm_tab[i].fr_dirty=0;
		frm_tab[i].cookie=NULL;				
		frm_tab[i].fr_loadtime=0;
		
		for(j=0;j<1024;j++)
		{
			
		  if(j<4 && ( j!=18 && i!=0) )
		  {
		pd_t  pd_temp = { 1,1,0,0,0,0,0,0,0,0, (1024 + j) };
		*pd_ptr= pd_temp;
		  }
		else
		{
		pd_t  pd_temp = {0,1,0,0,0,0,0,0,0,0,0};
		*pd_ptr= pd_temp;
		}
		  
		  pd_ptr++;

		}

	for(i=5;i<1024;i++)
	{
	  frm_tab[i].fr_status= FRM_UNMAPPED;
	  frm_tab[i].fr_pid=-1;				
  	  frm_tab[i].fr_vpno=0;				
	  frm_tab[i].fr_refcnt=0;			
	  frm_tab[i].fr_type=FR_PAGE;				
	  frm_tab[i].fr_dirty=0;
	  frm_tab[i].cookie=NULL;				
	  frm_tab[i].fr_loadtime=0;
	}


 // kprintf("To be implemented!:  %d\n",min_frame);
  return OK;
}

void first_ptentry(int a)
{
int i,j;

 pd_t *pd_ptr = (pd_t *) ((FRAME0 + a)*NBPG);

                frm_tab[a].fr_status= FRM_MAPPED;
             //   frm_tab[a].fr_pid=currpid;
                frm_tab[a].fr_vpno=0;
                frm_tab[a].fr_refcnt=0;
                frm_tab[a].fr_type=FR_DIR;
                frm_tab[a].fr_dirty=0;
                frm_tab[a].cookie=NULL;
                frm_tab[a].fr_loadtime=0;

                for(j=0;j<1024;j++)
                {

                 if(j<4)
                  {
                pd_t  pd_temp = { 1,0,0,0,0,0,0,0,0,0, (1024 + j) };
                *pd_ptr= pd_temp;
                  }
                else
                {
                pd_t  pd_temp = {0,1,0,0,0,0,0,0,0,0,0};
                *pd_ptr= pd_temp;
                }

                  pd_ptr++;

                }
}


/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */

SYSCALL get_frm(int* avail)
{

	int i,min=-1,min_frame=-1;
	
	if(grpolicy()==LRU)
	updateAccessTab();

	for(i=5;i<1024;i++)
	{
		 if(frm_tab[i].fr_status==FRM_UNMAPPED)
	           break;
	}
	if(i<1024)
	{
	*avail= ((FRAME0+i)<<12);
	min_frame=i;

//	kprintf("\n got page %d",*avail);
	}
	else
	{
		kprintf("\n\n Page Replacemtn \n");
	  if(grpolicy()==FIFO)
		{
		//	check_frm_status();
			min_frame = min_frm_status();

			
	//		kprintf("\nFIFO replace : %d  ",min_frm_status());
			*avail = ((FRAME0 + min_frame )<<12);
			
		}
		else if(grpolicy()==LRU)
		{
			/* LRU code */
			min_frame = min_frm_status();
	//		kprintf("\nLRU replace : %d     time: %d       Lcnt: 5d",(min_frame + FRAME0),frm_tab[min_frame].fr_loadtime, lru_counter);
			
                        *avail = ((FRAME0 + min_frame )<<12);
		}
	}

	frm_tab[min_frame].fr_status=FRM_MAPPED;
        frm_tab[min_frame].fr_pid=currpid;
        frm_tab[min_frame].fr_vpno=0;
       
	if(grpolicy()==FIFO)
	{
	 frm_tab[min_frame].fr_refcnt=fifo_counter;
	fifo_counter++;
	}

        frm_tab[min_frame].fr_type=FR_PAGE;
        frm_tab[min_frame].fr_dirty=0;
        frm_tab[min_frame].cookie=NULL;
        frm_tab[min_frame].fr_loadtime=0;
	
//	kprintf("\n got page %d   fifo:%d     pid:%d ",((*avail)>>12), frm_tab[min_frame].fr_refcnt,frm_tab[min_frame].fr_pid);
	  return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{

  

//  kprintf("To be implemented!\n");
  return OK;
}



