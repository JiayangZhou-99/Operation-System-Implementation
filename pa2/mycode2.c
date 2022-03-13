/* mycode2.c: your portion of the kernel
 *
 *   	Below are functions that are called by other parts of the kernel. 
 * 	Your ability to modify the kernel is via these functions.  You may
 *  	modify the bodies of these functions (and add code outside of them)
 *  	in any way you wish (however, you cannot change their interfaces).  
 */

#include "aux.h"
#include "sys.h"
#include "mycode2.h"

#define TIMERINTERVAL 1	// in ticks (tick = 10 msec)
#define LLMAX 18446744073709551615
/* 	A sample process table. You may change this any way you wish. 
 */

static struct {
	int valid;		// is this entry valid: 1 = yes, 0 = no
	int pid;		// process ID (as provided by kernel)
  unsigned long long int passValue; // process pass value for proportional
  int stride;
  int takeupCPU;
  int req; // 0 for 0 request, 1 for other request value
} proctab[MAXPROCS];

int head = MAXPROCS-1;
int tail = 0;
int curPid = -1;
int L=10000;
int avaliCPU = 100;
int totalCPU=100;
int unreqProc = 0;
int reqCPU = 0;
/* 	InitSched() is called when the kernel starts up. First, set the
 * 	scheduling policy (see sys.h). Make sure you follow the rules
 *   	below on where and how to set it.  Next, initialize all your data
 * 	structures (such as the process table).  Finally, set the timer
 *  	to interrupt after a specified number of ticks. 
 */

void InitSched()
{
	int i;

	/* First, set the scheduling policy. You should only set it
	 * from within this conditional statement. While you are working
	 * on this assignment, GetSchedPolicy() will return NOSCHEDPOLICY. 
	 * Thus, the condition will be true and you may set the scheduling
	 * policy to whatever you choose (i.e., you may replace ARBITRARY).  
	 * After the assignment is over, during the testing phase, we will
	 * have GetSchedPolicy() return the policy we wish to test (and
	 * the policy WILL NOT CHANGE during the entirety of a test).  Thus
	 * the condition will be false and SetSchedPolicy(p) will not be
	 * called, thus leaving the policy to whatever we chose to test
	 * (and so it is important that you NOT put any critical code in
	 * the body of the conditional statement, as it will not execute when
	 * we test your program). 
	 */
	if (GetSchedPolicy() == NOSCHEDPOLICY) {	// leave as is
							// no other code here
		SetSchedPolicy(PROPORTIONAL);		// set policy here
							// no other code here
	}
		
	/* Initialize all your data structures here */
	for (i = 0; i < MAXPROCS; i++) {
		proctab[i].valid = 0;
		proctab[i].passValue = 0;
		proctab[i].stride = 0;
		proctab[i].takeupCPU = 0;
		proctab[i].req = 0;

	}
	/* Set the timer last */
	SetTimer(TIMERINTERVAL);
}


/*  	StartingProc(p) is called by the kernel when the process
 * 	identified by PID p is starting. This allows you to record the
 * 	arrival of a new process in the process table, and allocate any
 * 	resources (if necessary). Returns 1 if successful, 0 otherwise. 
 */

int StartingProc(int p) 		
	// p: process that is starting
{
	int i;
  int j;
  switch (GetSchedPolicy()) {
    case ARBITRARY :
  	for (i = 0; i < MAXPROCS; i++) {
	  	if (! proctab[i].valid) {
		  	proctab[i].valid = 1;
		  	proctab[i].pid = p;
		  	return (1);
  		}
  	}
    break;
    case FIFO :
    case LIFO :
    case ROUNDROBIN :
    if(!proctab[tail].valid){
      proctab[(tail)%(MAXPROCS)].pid = p;
      proctab[(tail)%(MAXPROCS)].valid = 1;
      tail=(tail+1)%(MAXPROCS);
      DoSched();
    }
    return (1);
    break;
    case PROPORTIONAL :
    for (i = 0; i < MAXPROCS; i++) {
	  	if ( proctab[i].valid == 0) {
		  	proctab[i].valid = 1;
		  	proctab[i].pid = p;
        proctab[i].passValue = 0;
        proctab[i].req=0;
        unreqProc = unreqProc + 1;
		    for (j=0;j<MAXPROCS;j++) {  
          if (proctab[j].req==0 && proctab[j].valid==1) {
           proctab[j].takeupCPU = avaliCPU/unreqProc;
           proctab[j].stride = L/(proctab[j].takeupCPU);
          }
        }
        DoSched();
        return (1);
      }
  	}
    break;
  }
	DPrintf("Error in StartingProc: no free table entries\n");
	return(0);
}
			

/*   	EndingProc(p) is called by the kernel when the process
 * 	identified by PID p is ending.  This allows you to update the
 *  	process table accordingly, and deallocate any resources (if
 *  	necessary).  Returns 1 if successful, 0 otherwise. 
 */


int EndingProc(int p)
	// p: process that is ending
{
	int i;
  int j;
  switch (GetSchedPolicy()) {
    case PROPORTIONAL :
    for(i=0; i<MAXPROCS; i++) {
      if (proctab[i].valid && proctab[i].pid == p) {
        if (proctab[i].req == 0)  {
          unreqProc--;
        } else { 
          reqCPU = reqCPU - proctab[i].takeupCPU;
        }
        proctab[i].valid =0;
        proctab[i].passValue =0;
        proctab[i].stride =0;
        proctab[i].req=0;
        avaliCPU=avaliCPU+proctab[i].takeupCPU;
        proctab[i].takeupCPU=0;
        for (j=0; j<MAXPROCS; j++) {
         if (proctab[i].req==0 && proctab[i].valid==1) {
           proctab[i].takeupCPU = avaliCPU/unreqProc;
           proctab[i].stride = L/(proctab[i].takeupCPU);
          }
        }
        return (1);
      }
    }
    break;
    case ARBITRARY :
  	for (i = 0; i < MAXPROCS; i++) {
	  	if (proctab[i].valid && proctab[i].pid == p) {
		  	proctab[i].valid = 0;
	  		return(1);
  		}
  	}
    break;
    case FIFO :
    proctab[(head+1)%(MAXPROCS)].valid=0;
    head=(head+1)%(MAXPROCS);
    return(1);
    break;
    case LIFO :
    proctab[(tail-1)%(MAXPROCS)].valid=0;
    tail=(tail-1)%(MAXPROCS);
    if (tail<0) {
      tail =0;
      }
    return (1);
    break;
    case ROUNDROBIN:
    if (proctab[(head+1)%(MAXPROCS)].pid == p && proctab[(head+1)%(MAXPROCS)].valid) {
     proctab[(head+1)%(MAXPROCS)].valid=0;
     head = (head +1)%(MAXPROCS);
     return(1);
    }
    break;
  }

	DPrintf("Error in EndingProc: can't find process %d\n", p);
	return(0);
}


/* 	SchedProc() is called by kernel when it needs a decision for
 * 	which process to run next. It will call the kernel function
 * 	GetSchedPolicy() which will return the current scheduling policy
 *   	which was previously set via SetSchedPolicy(policy). SchedProc()
 * 	should return a process PID, or 0 if there are no processes to run. 
 */

int SchedProc()
{
	int i;
  int index;
  int minPass = LLMAX;
	switch(GetSchedPolicy()) {

	case ARBITRARY:

		for (i = 0; i < MAXPROCS; i++) {
			if (proctab[i].valid) {
				return(proctab[i].pid);
			}
		}
		break;

	case FIFO:

    return (proctab[(head+1)%(MAXPROCS)].pid); 
		break;

	case LIFO:
    DoSched();
		return (proctab[(tail-1)%(MAXPROCS)].pid);
		break;

	case ROUNDROBIN:
    curPid = proctab[(head+1)%(MAXPROCS)].pid;
    if (proctab[(head+1)%(MAXPROCS)].valid)
	    return(curPid);
		break;

	case PROPORTIONAL:
    for(i=0; i<MAXPROCS; i++) {
      if (minPass>proctab[i].passValue && proctab[i].valid) { 
        minPass=proctab[i].passValue;
        curPid = proctab[i].pid;
        index=i;
      }
    }
    if (minPass == LLMAX)
      return(0);
    proctab[index].passValue = proctab[index].passValue + proctab[index].stride;
   // DPrintf("PassValue of process %d is %lld,stride is %d\n",proctab[index].pid,proctab[index].passValue,proctab[index].stride);
   // DPrintf("pass %lld \n",(proctab[index].passValue));
    return(curPid);
		break;

	}
	
	return(0);
}


/*  	HandleTimerIntr() is called by the kernel whenever a timer
 *  	interrupt occurs.  Timer interrupts should occur on a fixed
 * 	periodic basis.
 */

void HandleTimerIntr()
{
  int i;
	SetTimer(TIMERINTERVAL);

	switch(GetSchedPolicy()) {	// is policy preemptive?

	case ROUNDROBIN:		// ROUNDROBIN is preemptive
  if ((curPid == proctab[(head+1)%(MAXPROCS)].pid) && proctab[(head+1)%(MAXPROCS)].valid) {
    proctab[(head+1)%(MAXPROCS)].valid=0;
    head = (head +1)%(MAXPROCS); 
    proctab[(tail)%(MAXPROCS)].pid = curPid;
    proctab[(tail)%(MAXPROCS)].valid = 1;
    tail=(tail+1)%(MAXPROCS);
  }
  DoSched();
  break;
	case PROPORTIONAL:		// PROPORTIONAL is preemptive
  //  for (i=0; i<MAXPROCS; i++) {
  //    if(proctab[i].pid == curPid && proctab[i].valid) {
  //      proctab[i].passValue = proctab[i].passValue + proctab[i].stride;
  //      DPrintf("passV of procs %d will add %d\n ",curPid,proctab[i].stride);
  //    }
  //  }
		DoSched();		// make scheduling decision
	break;

	default:			// if non-preemptive, do nothing
		break;
	}
}

/* 	MyRequestCPUrate(p, n) is called by the kernel whenever a process
 * 	identified by PID p calls RequestCPUrate(n).  This is a request for
 *   	n% of CPU time, i.e., requesting a CPU whose speed is effectively
 * 	n% of the actual CPU speed. Roughly n out of every 100 quantums
 *  	should be allocated to the calling process. n must be at least
 *  	0 and must be less than or equal to 100. MyRequestCPUrate(p, n)
 * 	should return 0 if successful, i.e., if such a request can be
 * 	satisfied, otherwise it should return -1, i.e., error (including
 * 	if n < 0 or n > 100). If MyRequestCPUrate(p, n) fails, it should
 *   	have no effect on the scheduling of this or any other process,
 * 	i.e., AS IF IT WERE NEVER CALLED.
 */

int MyRequestCPUrate(int p, int n)
	// p: process whose rate to change
	// n: percent of CPU time
{ 
   int i;
   int j;
   int tempCPU;
   if (n<0 || n>100|| GetSchedPolicy() != PROPORTIONAL)
   return(-1);

	 for (i=0; i < MAXPROCS; i++) {
      if ( n > 0 && proctab[i].valid && proctab[i].pid==p) {
	     if ( proctab[i].req ==0) {
         if ( reqCPU + n > totalCPU)
         return(-1);
         proctab[i].req=1;
         proctab[i].takeupCPU = n;
         proctab[i].stride=L/proctab[i].takeupCPU;
         unreqProc--;
         reqCPU= reqCPU + n;
         avaliCPU= totalCPU - reqCPU;
		     for (j=0;j<MAXPROCS;j++) { 
            if (proctab[j].req==0 && proctab[j].valid==1) {
              proctab[j].takeupCPU = avaliCPU/unreqProc;
              proctab[j].stride = L/(proctab[j].takeupCPU);   
            }
         }
         return (0);
        }
        else {
           if (reqCPU-proctab[i].takeupCPU + n > totalCPU)
           return (-1);
           tempCPU = proctab[i].takeupCPU;
           proctab[i].takeupCPU = n;
           proctab[i].stride=L/proctab[i].takeupCPU;
           reqCPU= reqCPU + n-tempCPU;
           avaliCPU= totalCPU-reqCPU;
		       for (j=0;j<MAXPROCS;j++) { 
              if (proctab[j].req==0 && proctab[j].valid==1) {
                proctab[j].takeupCPU = avaliCPU/unreqProc;
                proctab[j].stride = L/(proctab[j].takeupCPU);   
              }
           }
            return(0);
        }
       }
       else if ( n == 0 && proctab[i].valid && proctab[i].pid==p) {
          if (proctab[i].req==0) {
            return (-1);
          }
            proctab[i].req ==0;
            unreqProc++;
            reqCPU= reqCPU - proctab[i].takeupCPU;
            avaliCPU = totalCPU - reqCPU;
            for (j=0;j<MAXPROCS;j++) { 
              if (proctab[j].req==0 && proctab[j].valid==1) {
                proctab[j].takeupCPU = avaliCPU/unreqProc;
                proctab[j].stride = L/(proctab[j].takeupCPU);   
              }
            }
            return(0);   
       }
  	}
    return (-1);
}
