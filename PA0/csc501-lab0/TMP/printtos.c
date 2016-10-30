#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

static unsigned long *esp;
static unsigned long *ebp;

void printtos()
{

int i;
asm("movl %esp,esp");
asm("movl %ebp,ebp");

ebp+=2;

kprintf("\n\nPrev TOS Address:%x   Content:%ld",ebp,*ebp);
kprintf("\nCurrent TOS Address:%x   Content:%ld",esp,*esp);
kprintf("\n\nLast 6 Stack push:");

for(i=0;i<=5;i++)
kprintf("\nAddress:%x   Content:%ld",(esp+i),*(esp+i));

}

