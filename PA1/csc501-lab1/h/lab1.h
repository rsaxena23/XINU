#define LINUXSCHED 1
#define MULTIQSCHED 2

extern int epoc_time,epoc_time_real;
void setschedclass(int);
int getschedclass();
