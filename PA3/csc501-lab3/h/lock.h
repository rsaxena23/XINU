
#define NLOCKS 50
#define READ 0
#define WRITE 1
#define NO_OPR 2
#define LOCK_FREE 0
#define LOCK_ACQ 1

typedef struct qentry
{
  int pid;
  int prio;
  int operation;
  int waitTime;
  struct qentry *next;
} q_node;

typedef struct lentry {
   int lstate;
   int lock_opr;  
   q_node *start;
   int active;
   int lock_id;
} lock_map_t ;

/*typedef struct plock_list_entry {
  int lockno;
  struct plock_list_entry *next;
} plock_node; */

extern lock_map_t lock_tab[];

int lcreate();
int ldelete(int);
int lock(int,int,int);
void linit();
int releaseall(int,long,...);
