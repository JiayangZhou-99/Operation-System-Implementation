/* mycode1.c: your portion of the kernel
 *
 *   	Below are functions that are called by other parts of the kernel. 
 * 	Your ability to modify the kernel is via these functions.  You may
 *  	modify the bodies of these functions (and add code outside of them)
 *  	in any way you wish (however, you cannot change their interfaces).  
 */

#include <strings.h>
#include "aux.h"
#include "sys.h"
#include "mycode1.h"

/* 	NewContext(p, c) will be called by the kernel whenever a new
 * 	process	is created. This is essentially a notification (which you
 * 	will make use of) that this newly created process has an ID of p,
 *   	and that its initial context is pointed to by c. Make sure you
 * 	make a copy of the contents of this context (i.e., don't copy the
 *  	pointer, but the contents pointed to by the pointer), as the pointer
 *  	will become useless after this function returns.
 */
static CONTEXT con[MAXPROCS+1];
static int curid;

void NewContext(int p, CONTEXT *c)
	// p: ID of new process just created
	// c: initial context for this process
{
	/* your code here */
 // DPrintf("%d\n",p);
 memcpy(&con[p],c,sizeof(*c));//copy context from Process Memory Stack to Data 

}

/* 	MySwitchContext(p) should cause a context switch from the calling
 * 	process to process p. It should return the ID of the calling
 * 	process. The ID of the calling process can be determined by calling
 *   	GetCurProc(), which returns the currently running process's ID.  
 * 	The initial given implementation of MySwitchContext(p) makes use of
 *  	RefSwitchContext(p), which is a "reference" working version of the
 *  	kernel context switching function.  It is provided so that the kernel
 * 	can initially work without modification to allow the other exercises
 * 	to execute, and to illustrate proper behavior. For Exercise F, the
 * 	call to RefSwitchContext(p) MUST be removed, as your solution code
 *   	will effectively replace its functionality (and furthermore, it will
 * 	be deactivated during grading). 
 */
 
int MySwitchContext(int p)
	// p: ID of process to switch to
{
	/* your code here */

  int magic = 0;
  curid = GetCurProc();
  // DPrintf("%d\n",curid);
  SaveContext(&con[curid]);
  if (magic == 1) return curid;
    else magic = 1;
  RestoreContext(&con[p]);

  //return(RefSwitchContext(p));	// remove call to RefSwitchContext(p)
}
