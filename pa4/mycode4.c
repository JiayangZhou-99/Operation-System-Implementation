/* mycode4.c: UMIX thread package
 *
 *   	Below are functions that comprise the UMIX user-level thread package. 
 * 	These functions are called by user programs that use threads.  You may
 *  	modify the bodies of these functions (and add code outside of them)
 *  	in any way you wish (however, you cannot change their interfaces).  
 */

#include <setjmp.h>
#include <string.h>
#include "aux.h"
#include "umix.h"
#include "mycode4.h"


static int MyInitThreadsCalled = 0;	// 1 if MyInitThreads called, else 0
static int lastcreate = 0;
static int head = -1;
static int tail =-1;
static int curthread =-1;
static int prevthread;
static struct thread {			// thread table
	int valid;			// 1 if entry is valid, else 0
	jmp_buf env;			// current context
  jmp_buf initpos;
  int prev;
  int next;
  int schedflag;
  void(*func)();
  int param;
} thread[MAXTHREADS];

#define STACKSIZE	65536		// maximum size of thread stack

/* 	MyInitThreads() initializes the thread package. Must be the first
 * 	function called by any user program that uses the thread package. 
 */
static int isempty_queue() {
  if (head == -1 ) return (1);
  else return (0);
}
static void printQ(){
  if(head==-1)return;
  int ptr =head;
  while(ptr!=tail) {
    DPrintf("%d->",ptr);
    ptr=thread[ptr].next;
  }
  DPrintf("%d \n",ptr);
}
static void push_queue(int tid) {
  if (head==-1) {
  head=tid;
  tail=tid;
  }else {
  thread[tail].next=tid;
  thread[tid].prev=tail;
  tail=tid;
  }
}
static int pop_queue() {
  int p;
  if (head == -1 ) {
    return -1;
  }else if (head == tail) {
    p = head;
    head = -1;
    tail = -1;
  }else{
    p = head;
    thread[thread[p].next].prev=-1;
    head=thread[p].next;
    thread[p].next=-1;
  }
  return p;
}
static void pickout(int tid) {
  if (head == -1) return;
  if (head==tid) pop_queue();
  else if (tid == tail) {
    tail=thread[tid].prev;
    thread[tid].prev=-1;
    thread[tid].next=-1;
    }
  else {
    thread[thread[tid].prev].next=thread[tid].next;
    thread[thread[tid].next].prev=thread[tid].prev;
    thread[tid].prev=-1;
    thread[tid].next=-1;
  }
}

void MyInitThreads()
{
	int i;

	if (MyInitThreadsCalled) {		// run only once
		Printf("MyInitThreads: should be called only once\n");
		Exit();
	}

	for (i = 0; i < MAXTHREADS; i++) {	// initialize thread table
		thread[i].valid = 0;
    thread[i].prev = -1;
    thread[i].next = -1;
    thread[i].schedflag = 0;
    thread[i].func = 0;
    thread[i].param = -1;
    char stack[i*STACKSIZE];
		if (((int) &stack[i*STACKSIZE-1]) - ((int) &stack[0]) + 1 !=i*STACKSIZE) {
			Printf("Stack space reservation failed\n");
			Exit();
		}
    if(setjmp(thread[i].env)!=0) {
      if(thread[curthread].valid) {
      thread[curthread].func(thread[curthread].param);
      }
      MyExitThread();
    }else {
       memcpy(thread[i].initpos,thread[i].env,sizeof(jmp_buf));
    }
	}
	thread[0].valid = 1;			// initialize thread 0
  lastcreate=0;
  curthread=0;
 /* for (i=0; i< MAXTHREADS; i++){ 
   char stack[i*STACKSIZE];
	  	if (((int) &stack[i*STACKSIZE-1]) - ((int) &stack[0]) + 1 !=i*STACKSIZE) {
		  	Printf("Stack space reservation failed\n");
		  	Exit();
  		}
      if(setjmp(thread[i].env)!=0) {
        if(thread[curthread].valid) {
        thread[curthread].func(thread[curthread].param);
        }
        MyExitThread();
      }else {
        memcpy(thread[i].initpos,thread[i].env,sizeof(jmp_buf));
      }
  }*/
	MyInitThreadsCalled = 1;
}

/* 	MyCreateThread(f, p) creates a new thread to execute f(p),
 *   	where f is a function with no return value and p is an
 * 	integer parameter. The new thread does not begin executing
 *  	until another thread yields to it. 
 */

int MyCreateThread(void (*f)(), int p)
	// f: function to be executed
	// p: integer parameter
{
  int i;
	if (! MyInitThreadsCalled) {
		Printf("MyCreateThread: Must call MyInitThreads first\n");
		Exit();
	}
  for (i=1;i<=MAXTHREADS;i++) {
   lastcreate=(lastcreate+1)%(MAXTHREADS);
   if (thread[lastcreate].valid == 0) {
    thread[lastcreate].func=f;
    thread[lastcreate].param=p;
    thread[lastcreate].valid=1;
    memcpy(thread[lastcreate].env,thread[lastcreate].initpos,sizeof(jmp_buf));
    push_queue(lastcreate);
    return lastcreate;
   }
  }
  return -1;
}
/*  	MyYieldThread(t) causes the running thread, call it T, to yield to
 * 	thread t.  Returns the ID of the thread that yielded to the calling
 * 	thread T, or -1 if t is an invalid ID.  Example: given two threads
 * 	with IDs 1 and 2, if thread 1 calls MyYieldThread(2), then thread 2
 *   	will resume, and if thread 2 then calls MyYieldThread(1), thread 1
 * 	will resume by returning from its call to MyYieldThread(2), which
 *  	will return the value 2.
 */

int MyYieldThread(int t)
	// t: thread being yielded to
{
	if (! MyInitThreadsCalled) {
		Printf("MyYieldThread: Must call MyInitThreads first\n");
		Exit();
	}

	if (t < 0 || t >= MAXTHREADS) {
		Printf("MyYieldThread: %d is not a valid thread ID\n", t);
		return(-1);
	}
	if (! thread[t].valid) {
		Printf("MyYieldThread: Thread %d does not exist\n", t);
		return(-1);
	}
  if (t==curthread) return t;
  if (setjmp(thread[curthread].env) == 0) {
    if(thread[curthread].valid) {
      push_queue(curthread);
    }
    prevthread=curthread;
    curthread=t;
    if (thread[t].schedflag!=1) {
      thread[t].schedflag=0;
      pickout(t);
    }
    longjmp(thread[t].env, 1);
  }
  if (thread[curthread].schedflag) {
    thread[curthread].schedflag=0;
    return -1;
  }
  else return prevthread;
}

/*  	MyGetThread() returns ID of currently running thread. 
 */

int MyGetThread()
{
	if (! MyInitThreadsCalled) {
		Printf("MyGetThread: Must call MyInitThreads first\n");
		Exit();
	}
  return curthread;
}

/* 	MySchedThread() causes the running thread to simply give up the
 * 	CPU and allow another thread to be scheduled. Selecting which
 * 	thread to run is determined here. Note that the same thread may
 *   	be chosen (as will be the case if there are no other threads). 
 */

void MySchedThread()
{
  int t;
	if (! MyInitThreadsCalled) {
		Printf("MySchedThread: Must call MyInitThreads first\n");
		Exit();
	}
  if (head==-1) MyYieldThread(curthread);
  else {
    t=pop_queue();
    thread[t].schedflag=1;
    MyYieldThread(t);

  } 
}

/* 	MyExitThread() causes the currently running thread to exit.  
 */

void MyExitThread()
{
	if (! MyInitThreadsCalled) {
		Printf("MyExitThread: Must call MyInitThreads first\n");
		Exit();
	}
   // DPrintf("before exit queue is ");
  //  printQ();
  thread[curthread].valid=0;
  thread[curthread].func=0;
  thread[curthread].param=-1;
  thread[curthread].schedflag=1;
  if (head==-1) Exit();
  else MySchedThread();
}
