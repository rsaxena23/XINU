
extern int etext;
extern int edata;
extern int end;

void printsegaddress()
{
     int *content_pt;
	 content_pt=&etext;
	 kprintf("\n\nEnd of text Address:%x",&etext);
     kprintf("\n4byte previous Content:%x",*(content_pt-1));
     kprintf("\n4byte next Content:%x \n",*(content_pt+1));
   
   content_pt=&edata;
	    kprintf("\nEnd of data Address:%x",&edata);
   kprintf("\n4byte previous Content:%x",*(content_pt-1));
     kprintf("\n4byte next Content:%x \n",*(content_pt+1));
   
   content_pt=&end;
	kprintf("\nEnd of bss Address:%x",&end);
   kprintf("\n4byte previous Content:%x",*(content_pt-1));
   kprintf("\n4byte next Content:%x \n",*(content_pt+1));
}

