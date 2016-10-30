#include<conf.h>
#include<kernel.h>
#include<stdio.h>
#include<lock.h>
#include<proc.h>

int lcreate()
{
int i;
for(i=0;i<NLOCKS;i++)
{
   if(lock_tab[i].lstate==LOCK_FREE)
   break;
}

if(i<NLOCKS)
{
  lock_tab[i].lstate= LOCK_ACQ;
//return i;
   lock_tab[i].lock_id+=NLOCKS;
   return lock_tab[i].lock_id;
}
  
  else  
  return SYSERR;
}

