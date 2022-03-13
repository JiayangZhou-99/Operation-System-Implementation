/* Programming Assignment 1: Exercise C
 *
 * Now we will learn how to effect the execution sequence of the various
 * processes.  We introduce a new system call, Yield(pid), which causes
 * the currently running process to yield to the process whose identifier
 * is pid.  Furthermore, when Yield(pid) returns, it returns the identifier
 * of the process that yielded to the one that is now running (and returning
 * from Yield(pid)).  This is illustrated by the program below, which you
 * should study and run.
 *
 * Questions
 *
 * 1. Can you explain the order of what gets printed based on the code?
 *
 */

#include <stdio.h>
#include "aux.h"
#include "umix.h"

void Main()
{
	int pid = 0, rpid;

	if ((pid = Fork()) == 0) {

		/* first child executes here */
		Printf("I am the first child, my pid is %d\n", Getpid());
		Exit();
	}

	Printf("I am the parent, my pid is %d\n", Getpid());

	Printf("About to yield to child process whose pid is %d\n", pid);
	rpid = Yield(pid);	// yield to first child before continuing
	Printf("Process %d just yielded to me (the parent)\n", rpid);

	if ((pid = Fork()) == 0) {

		/* second child executes here */
		Printf("I am the second child, my pid is %d\n", Getpid());
		Exit();
	}

	Printf("About to yield to child process whose pid is %d\n", pid);
	rpid = Yield(pid);	// yield to second child before continuing
	Printf("Process %d just yielded to me (the parent)\n", rpid);
}
