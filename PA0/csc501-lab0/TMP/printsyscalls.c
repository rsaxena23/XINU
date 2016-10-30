#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

static int tswitch=0;

void printsyscalls()
{
int i,j;
if(tswitch==1)
{
for(i=0;i<NPROC;i++)
{
    if(systrack[i].proc_exc=='Y')
	{	
  kprintf("\n\n Values for Process:%d\n",i);
			kprintf(" GETPID:%d",systrack[i].call_count[0]);
			kprintf(" GETTIME:%d",systrack[i].call_count[1]);
			kprintf(" KILL:%d",systrack[i].call_count[2]);
			kprintf(" SIGNAL:%d",systrack[i].call_count[3]);
			kprintf(" SLEEP:%d",systrack[i].call_count[4]);
			 kprintf(" WAIT:%d",systrack[i].call_count[5]);
	}
}
}

}

void syscallsummary_start()
{
int i,j;

for(i=0;i<NPROC;i++)
{   
   systrack[i].proc_exc='N';
for(j=0;j<TOTAL_FUNCTIONS;j++)
{
  systrack[i].call_count[j]=0;
}
}

tswitch=0;
}

void syscallsummary_stop()
{
tswitch=1;
}


