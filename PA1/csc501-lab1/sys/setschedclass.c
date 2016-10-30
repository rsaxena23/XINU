#include<conf.h>
#include<stdio.h>
#include<kernel.h>
#include<proc.h>
#include<lab1.h>

extern int scheduler_type=1;
int epoch_time;
extern int multiq;

void setschedclass(int schedset)
{
  scheduler_type=schedset;

if(schedset==LINUXSCHED)
multiq=1;

else
multiq=0;

}

int getschedclass()
{
return scheduler_type;
}
