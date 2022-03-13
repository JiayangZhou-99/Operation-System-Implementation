#include "aux.h"
#include "umix.h"
#include "mycode4.h"
#include <setjmp.h>
#include "mycode4.c"

void Main()
{
  int Q[9];
  MyInitThreads();

  for (int index =1; index<=10;index++) {
    push_queue(index);
  }
  printQ();
//  for (int index =2; index<=9;index++) {
    pickout(1);
    pickout(10);
//  }
  printQ();
//  int k =pop_queue();
//  DPrintf("dequeue %d\n",k);

  jmp_buf env;
  int t = 1;
  int Setjmp(), Setjmp1(), Longjmp(), Longjmp1();
/*
  Printf("A: t = %d\n", t);			// Point A

  if ((t = Setjmp(env)) == 0) {		// conditional test
    t = 2;
    Printf("B: t = %d\n", t);		// Point B
    longjmp(env, t);
  } else {
    t = t + 2;
    Printf("C: t = %d\n", t);		// Point C
  }
  t = t + 1;
  Printf("D: t = %d\n", t);			// Point D
  */
}

int Setjmp(jmp_buf env)
  // env: to contain saved state
{
  Printf("Inside Setjmp\n");
  return(Setjmp1(env));
}

int Setjmp1(jmp_buf env)
  // env: to contain saved state
{
  Printf("Inside Setjmp1\n");
  return(setjmp(env));
}

int Longjmp(jmp_buf env, int t)
  // env: state to restore
  // t: thread to resume
{
  Printf("Inside Longjmp\n");
  Longjmp1(env, t);
}

int Longjmp1(jmp_buf env, int t)
  // env: state to restore
  // t: thread to resume
{
  Printf("Inside Longjmp1\n");
  longjmp(env, t);
}
