/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int epoch_time=0,epoch_time_real=0;
int scheduler_type;
int multiq;
int linux_run=0,multiq_run=0,selector=0;





int resched()
{

	int i,max_goodness,exec_process,dontswitch=0,quantum_value=0,process_ex;
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than next*/

	optr= &proctab[currpid];


 
/*

	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		return(OK);
	}  */
	
	/* force context switch      */

/*	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}  */

   //  ###################  LINUXSCHED #########################

	if(scheduler_type==LINUXSCHED)
	{
//	kprintf(" Main:%d",proctab[49].pstate);
	if(epoch_time==0)
	{
				
 //          kprintf("\nNew Epoch:\n");
           
 		for(i=0;i<NPROC;i++)
		{
	 	proctab[i].isinepoch=1;
		}          

	  	for(i=1;i<NPROC;i++)
		{
		 
		  if((proctab[i].pstate==PRREADY || proctab[i].pstate==PRCURR) && proctab[i].isReal==1)
                        {                        
			proctab[i].isinepoch= 0;
	//		kprintf("\n %d: Pcnt:%d",i,proctab[i].pcounter);
			if(proctab[i].pcounter>0)
			proctab[i].pcounter+=proctab[i].pprio;

			else
			proctab[i].pcounter=proctab[i].pprio;

			epoch_time+= proctab[i].pprio;			
	//		kprintf("\n%s:%d is in epoch, counter:%d ",proctab[i].pname,i,proctab[i].pcounter);
			}
		  if(proctab[i].pstate!=PRFREE)
			{
//				kprintf("\n%s:%d   state:%d",proctab[i].pname,i,proctab[i].pstate);
				}
		}
//	   	kprintf("\n\nepoch:%d\n",epoch_time);
	}
        
           max_goodness=0;
          exec_process=0;

/*		for(i=1;i<NPROC;i++)
		{
		  if(proctab[i].pstate==PRREADY)
	 	  kprintf("\n%d is ready\n",i);
		} */

//		kprintf("\nEpoch time:%d\n",epoch_time);
		for(i=1;i<NPROC;i++)
		{
			if(proctab[i].isinepoch==0 || proctab[i].pstate==PRREADY || proctab[i].pstate==PRCURR)
			{
                            if((proctab[i].pprio + proctab[i].pcounter)>max_goodness && proctab[i].pcounter!=0)
				{
					exec_process=i;
					max_goodness = (proctab[i].pprio + proctab[i].pcounter);				
				}
			}
		}


 //kprintf("\n\nGDof %d = %d counter=%d  left_epoch:%d pstate:%d\n",exec_process,max_goodness,proctab[exec_process].pcounter,epoch_time,proctab[exec_process].pstate);
// kprintf("\n Exc %d:%d st:%d ep:%d",exec_process,proctab[exec_process].pcounter,proctab[exec_process].pstate,epoch_time);


	if(exec_process==0)
	{ 

//	kprintf(" NULL ");
	for(i=0;i<NPROC;i++)
	{
	   if(proctab[i].isinepoch==0 && proctab[i].pcounter>0 && proctab[i].isReal==1)
		{
	//	kprintf("\n SC-New Ep:");
		epoch_time=0;
		resched();
		return OK;
		}	
	}

//	kprintf(" NULL ");
        return OK;
	}

/*	if(epoch_time>=10)
	{
		proctab[exec_process].pcounter -= 10;
		epoch_time-=10;
	//	kprintf("\n Decremented %d",exec_process);
	}
	else
{
	epoch_time=0;
	resched();
}*/

	if(proctab[exec_process].pcounter>=10)
	{
	quantum_value=10;
	}
	else
	quantum_value=proctab[exec_process].pcounter;

	 proctab[exec_process].pcounter -= quantum_value;
                epoch_time-= quantum_value;

//kprintf(" Dec");

//kprintf("\n Ex:%d Eptime: %d");

/*	else
	{              
		epoch_time-=proctab[exec_process].pcounter;
		proctab[exec_process].pcounter=0;
		kprintf(" Decremented 2\n");
	} */


	/* remove highest priority process at end of ready list */

//	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	if(currpid==exec_process)
	{
	dontswitch=1;
	}
	else
	{
	dontswitch=0;
	 if(proctab[currpid].pstate==PRCURR)
	{
	proctab[currpid].pstate=PRREADY;
	}
}
	
	
	nptr = &proctab[(currpid=exec_process)];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
//	enqueue(exec_process,rdytail);
#ifdef	RTCLOCK
	preempt = quantum_value;		/* reset preemption counter	*/
#endif
	
	if(dontswitch!=1)
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	//kprintf(" \n Epoch_time: %d");
	
	if(epoch_time==0 && multiq==0)
	scheduler_type=MULTIQSCHED;

	else
	scheduler_type=LINUXSCHED;

//	kprintf("\n scheduler type:%d",scheduler_type);

return OK;

	}


		// ################ MULTIQ ###################

	if(scheduler_type==MULTIQSCHED)
	{
	//randomness select

	if(epoch_time>0)
	{
	 scheduler_type=LINUXSCHED;
        linux_run++;
     //   kprintf("\n Linux: 1 ");
	resched();
        return OK;
	}
	//  srand(1);
	selector=rand()%10;
//	kprintf("\nRandom: %d   ep:%d  epr:%d",selector,epoch_time,epoch_time_real);
	
	if(selector<3 && epoch_time_real==0)
	{
	scheduler_type=LINUXSCHED;

//	linux_run++;
//	kprintf("\n Linux: 2 ");
	resched();	
	return OK;
	
	}

	else
	{

//	kprintf("\n Multiq: sl:%d  epr:%d",selector,epoch_time_real);


	  if(epoch_time_real==0)
	{
	multiq_run++;

	for(i=1;i<NPROC;i++)
		{
		proctab[i].isinepoch=1;
			if(proctab[i].isReal==0 && (proctab[i].pstate==PRREADY || proctab[i].pstate==PRCURR))
			{
		//	kprintf("\n %d: is in Real Epoch ",i);
			epoch_time_real+=100;
			proctab[i].isinepoch=0;
			}
		}
	}

	process_ex=0;

	for(i=1;i<NPROC;i++)
		{
		if(proctab[i].isinepoch==0 && proctab[i].isReal==0 && (proctab[i].pstate==PRREADY || proctab[i].pstate==PRCURR))
			{
		//	kprintf("\nStarted:");			
			process_ex=i;						
			break;
			}
		}
	if(process_ex!=0)
{
	nptr =&proctab[process_ex];
	currpid=process_ex;
	int dontswitch=1;

	if(optr==nptr)
	dontswitch=0;

	else
	{
        	if(optr->pstate==PRCURR)
                optr->pstate=PRREADY;

   	nptr->pstate=PRCURR;
	}

	nptr->isinepoch=1;

	if(epoch_time_real>0)
	epoch_time_real-=100;

	#ifdef RTCLOCK
	preempt=100;
	#endif

//	kprintf("\n M-Exec:%d  ep:%d",currpid,epoch_time_real);

	if(dontswitch==1)
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
}
	return OK;

	}

	}

	return OK;

	/* The OLD process returns here when resumed. */
	//return OK;
	
}



