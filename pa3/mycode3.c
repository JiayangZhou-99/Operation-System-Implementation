/* mycode3.c: your portion of the kernel
 *
 *   	Below are functions that are called by other parts of the kernel. 
 * 	Your ability to modify the kernel is via these functions.  You may
 *  	modify the bodies of these functions (and add code outside of them)
 *  	in any way you wish (however, you cannot change their interfaces).  
 */

#include "aux.h"
#include "sys.h"
#include "mycode3.h"

#define FALSE 0
#define TRUE 1

/* 	A sample semaphore table. You may change this any way you wish. 
 */

static struct {
	int valid;	// Is this a valid entry (was sem allocated)?
	int value;// value of semaphore
  int blocktab[MAXPROCS];
  int head;
  int tail;
} semtab[MAXSEMS];

/* 	InitSem() is called when kernel starts up. Initialize data
 * 	structures (such as the semaphore table) and call any initialization
 *   	functions here. 
 */

void InitSem()
{
	int s;
  int i;
	/* modify or add code any way you wish */

	for (s = 0; s < MAXSEMS; s++) {		// mark all sems free
		semtab[s].valid = FALSE;
    semtab[s].head = MAXPROCS-1;
    semtab[s].tail = 0;
    for (i = 0; i < MAXPROCS; i++) {
    semtab[s].blocktab[i]= -1;
    }
  }
}

/* 	MySeminit(v) is called by the kernel whenever the system call
 *  	Seminit(v) is called.  The kernel passes the initial value v.  
 *  	MySeminit should allocate a semaphore (find a free entry in
 * 	semtab and allocate), initialize that semaphore's value to v,
 * 	and then return the ID (i.e., index of the allocated entry). 
 */

int MySeminit(int v)
	// v: initial value of semaphore
{
	int s;

	/* modify or add code any way you wish */

	for (s = 0; s < MAXSEMS; s++) {
		if (semtab[s].valid == FALSE) {
			break;
		}
	}
	if (s == MAXSEMS) {
		DPrintf("No free semaphores\n");
		return(-1);
	}

	semtab[s].valid = TRUE;
	semtab[s].value = v;

	return(s);
}

/* 	MyWait(s) is called by the kernel whenever the system call
 *   	Wait(s) is called. 
 */

void MyWait(int s)
	// s: semaphore ID
{
	/* modify or add code any way you wish */
  int curPid;
  curPid = GetCurProc();
	semtab[s].value--;
  if (semtab[s].value<0) {
    semtab[s].blocktab[semtab[s].tail]=curPid;
    semtab[s].tail =(semtab[s].tail+1) % MAXPROCS;
    Block();  
  }
}

/* 	MySignal(s) is called by the kernel whenever the system call
 *  	Signal(s) is called. 
 */

void MySignal(int s)
	// s: semaphore ID
{
	/* modify or add code any way you wish */
  int readyPid;
	semtab[s].value++;
  if(semtab[s].blocktab[(semtab[s].head+1)% MAXPROCS] != -1) {
    readyPid=semtab[s].blocktab[(semtab[s].head+1) % MAXPROCS];
    semtab[s].head = (semtab[s].head+1)% MAXPROCS;
    semtab[s].blocktab[semtab[s].head] = -1;
    Unblock(readyPid);
  }

}
