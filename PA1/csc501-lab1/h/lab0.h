#ifndef _LAB0_H_
#define _LAB0_H_
#define GETPID  0
#define GETTIME 1
#define KILL    2
#define SIGNAL  3
#define SLEEP   4
#define WAIT    5
#define TOTAL_FUNCTIONS 6
 struct tracker {
            int call_count[TOTAL_FUNCTIONS];
			char proc_exc;
			};		
struct tracker systrack[NPROC];
#endif

long zfunction(long);
void printsegaddress();
void printtos();
void printprocstks(int);
void printsyscalls();
void syscallsummary_start();
void syscallsummary_stop();
