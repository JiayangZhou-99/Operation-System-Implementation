
#include <stdio.h>
#include "aux.h"
#include "sys.h"
#include "umix.h"

void InitRoad(void);
void driveRoad(int from, int mph);
void test1();

struct {
  int road_dir;
  int cars_on_road;
  int east_wait;
  int east_wait_cnt;
  int west_wait;
  int west_wait_cnt;
  int pos[NUMPOS+1];
  int mutex;
  int sempos[NUMPOS+1];
}shm;

void test1()
{
  if (Fork() == 0) {
    Delay(1200);			// car 2
    driveRoad(WEST, 90);
    Exit();
  }

  if (Fork() == 0) {
    Delay(900);			// car 3
    driveRoad(EAST, 50);
    Exit();
  }

  if (Fork() == 0) {
    Delay(900);			// car 4
    driveRoad(WEST, 30);
    Exit();
  }

  driveRoad(EAST, 40);			// car 1

  Exit();
}


void test2()
{
  if (Fork() == 0) {
    driveRoad(EAST, 20);
    Exit();
  }

  if (Fork() == 0) {
    Delay(10);			// car 3
    driveRoad(EAST, 20);
    Exit();
  }

  if (Fork() == 0) {
    Delay(20);			// car 4
    driveRoad(EAST, 40);
    Exit();
  }
  if (Fork() == 0) {
    Delay(40);			// car 4
    driveRoad(EAST, 40);
    Exit();
  }
  if (Fork() == 0) {
    Delay(300);			// car 4
    driveRoad(WEST, 40);
    Exit();
  }
}

void test3() {
  for(int i =2; i<= 10; ++i){
    if(Fork()==0) {
      Delay(i*5);
      driveRoad(WEST,(i % 2 ==0 ? 40:20));
      Exit();
    }
  }

  driveRoad(WEST,40); // car 1
  Exit();
}

void test4() {
  for(int i =2; i<= 5; ++i){
    if(Fork()==0) {
      Delay(100);
      driveRoad(EAST,40);
      Exit();
    }
  }
  for(int i =6; i<= 9; ++i){
    if(Fork()==0) {
      Delay(100);
      driveRoad(WEST,40);
      Exit();
    }
  }

  driveRoad(WEST,40); // car 1
  Exit();
}


void test5()
{
  DPrintf("TEST: Car 1 from west\n");

  DPrintf("\n>> 1 arrives from WEST\n\n");
  driveRoad(WEST, 40);
}

void test6()
{
  DPrintf("TEST: Car 1 from east\n");
  DPrintf("\n>> 1 arrives from EAST\n\n");
  driveRoad(EAST, 40);
}

void test7()
{
  DPrintf("TEST: Car 1 and 2 (faster) from west, should be able to go both without crashing\n");

  if (Fork() == 0) {
    DPrintf("\n>> 2 arrives from WEST\n\n");
    driveRoad(WEST, 60);
    Exit();
  }
  DPrintf("\n>> 1 arrives from WEST\n\n");
  driveRoad(WEST, 40);
}

void test8()
{
  DPrintf("TEST: Car 1 from west and 2 from east, 1 should go first then 2\n");

  if (Fork() == 0) {
    DPrintf("\n>> 2 arrives from EAST\n\n");
    driveRoad(EAST, 60);
    Exit();
  }
  DPrintf("\n>> 1 arrives from WEST\n\n");
  driveRoad(WEST, 40);
}

void test9()
{
  DPrintf("TEST: Car 1 from west and then with some delay 2 from east (while 1 is still on the road), 1 go first then 2\n");

  if (Fork() == 0) {
    Delay(500);
    DPrintf("\n>> 2 arrives from EAST\n\n");
    driveRoad(EAST, 60);
    Exit();
  }
  DPrintf("\n>> 1 arrives from WEST\n\n");
  driveRoad(WEST, 40);
}

void test10()
{
  DPrintf("TEST: Car 1 from west, out of road, then car 2 arrives east, 1 go first then 2\n");

  if (Fork() == 0) {
    Delay(1000);
    DPrintf("\n>> 2 arrives from EAST\n\n");
    driveRoad(EAST, 60);
    Exit();
  }
  DPrintf("\n>> 1 arrives from WEST\n\n");
  driveRoad(WEST, 40);


}

void test11()
{
  DPrintf("TEST: Car 1 from west, car 2 arrives while 1 on road, car 3 arrives while 2 is on road and 1 is already out, should be able to go\n");

  if (Fork() == 0) {
    Delay(200);
    DPrintf("\n>> 2 arrives from EAST\n\n");
    driveRoad(EAST, 40);
    Exit();
  }

  if (Fork() == 0) {
    Delay(1000);
    DPrintf("\n>> 3 arrives from EAST\n\n");
    driveRoad(EAST, 60);
    Exit();
  }

  DPrintf("\n>> 1 arrives from WEST\n\n");
  driveRoad(WEST, 40);
}

// Same as pa3d original test. 
// NOTE: Result can be different between each run
void test12()
{
  DPrintf("TEST: 1 from east, 3 and 4 arrive east and west same time, then 2 arrives west\nNOTE: Result can be DIFFERENT between each run\n");
  if (Fork() == 0) {
    Delay(1200);      // car 2
    DPrintf("\n>> 2 arrives from WEST\n\n");
    driveRoad(WEST, 60);
    Exit();
  }

  if (Fork() == 0) {
    Delay(900);     // car 3
    DPrintf("\n>> 3 arrives from EAST\n\n");
    driveRoad(EAST, 50);
    Exit();
  }

  if (Fork() == 0) {
    Delay(900);     // car 4
    DPrintf("\n>> 4 arrives from WEST\n\n");
    driveRoad(WEST, 30);
    Exit();
  }

  DPrintf("\n>> 1 arrives from EAST\n\n");
  driveRoad(EAST, 40);      // car 1
}

void test13()
{
  DPrintf("TEST: 1 from west, then A LOT OF CARS arrive east.\n");

  if (Fork() == 0) {
    Delay(400);
    DPrintf("\n>> 2 arrives from EAST\n\n");
    driveRoad(EAST, 100);
    Exit();
  }

  if (Fork() == 0) {
    Delay(500);
    DPrintf("\n>> 3 arrives from EAST\n\n");
    driveRoad(EAST, 50);
    Exit();
  }

  if (Fork() == 0) {
    Delay(600);
    DPrintf("\n>> 4 arrives from EAST\n\n");
    driveRoad(EAST, 50);
    Exit();
  }

  if (Fork() == 0) {
    Delay(600);
    DPrintf("\n>> 5 arrives from EAST\n\n");
    driveRoad(EAST, 100);
    Exit();
  }

  if (Fork() == 0) {
    Delay(700);
    DPrintf("\n>> 6 arrives from EAST\n\n");
    driveRoad(EAST, 150);
    Exit();
  }

  if (Fork() == 0) {
    Delay(720);
    DPrintf("\n>> 7 arrives from EAST\n\n");
    driveRoad(EAST, 100);
    Exit();
  }

  if (Fork() == 0) {
    Delay(730);
    DPrintf("\n>> 8 arrives from EAST\n\n");
    driveRoad(EAST, 100);
    Exit();
  }

  if (Fork() == 0) {
    Delay(740);
    DPrintf("\n>> 9 arrives from EAST\n\n");
    driveRoad(EAST, 70);
    Exit();
  }

  DPrintf("\n>> 1 arrives from WEST\n\n");
  driveRoad(WEST, 40);
}

// Car 1 slow from west, 
// 2, 3, 4 closely from east
// Car 5 then enter from west, should go after 2
// 5 |----1>-----| 2,3,4
// Should be 1,2,5,3,4
void test14()
{
  DPrintf("TEST: 1 slowly from west, 2,3,4 closely arrive east, 5 then enter from west, should go after 2, i.e. 1,2,5,3,4\n");
  if (Fork() == 0) {
    Delay(400);
    DPrintf("\n>> 2 arrives from EAST\n\n");
    driveRoad(EAST, 100);
    Exit();
  }

  if (Fork() == 0) {
    Delay(500);
    DPrintf("\n>> 3 arrives from EAST\n\n");
    driveRoad(EAST, 50);
    Exit();
  }

  if (Fork() == 0) {
    Delay(600);
    DPrintf("\n>> 4 arrives from EAST\n\n");
    driveRoad(EAST, 50);
    Exit();
  }

  if (Fork() == 0) {
    Delay(700);
    DPrintf("\n>> 5 arrives from WEST\n\n");
    driveRoad(WEST, 50);
    Exit();
  }

  DPrintf("\n>> 1 arrives from WEST\n\n");
  driveRoad(WEST, 30);
}

// Car 1 slow from west, 
// 2, 3, 4 closely from east
// Car 5, 6, 7 then enter from west
// Should be 1, 2, 5, 3, 6, 4, 7
// 7,6,5 |----1>-----| 2,3,4
// Road Trace: >|<|>|<|>|<|>|
void test15()
{
  DPrintf("TEST: 1 slowly from west, 2,3,4 closely from east, 5,6,7 closely from west, should take turn.\n");

  if (Fork() == 0) {
    Delay(400);
    DPrintf("\n>> 2 arrives from EAST\n\n");
    driveRoad(EAST, 100);
    Exit();
  }

  if (Fork() == 0) {
    Delay(500);
    DPrintf("\n>> 3 arrives from EAST\n\n");
    driveRoad(EAST, 50);
    Exit();
  }

  if (Fork() == 0) {
    Delay(600);
    DPrintf("\n>> 4 arrives from EAST\n\n");
    driveRoad(EAST, 50);
    Exit();
  }

  if (Fork() == 0) {
    Delay(700);
    DPrintf("\n>> 5 arrives from WEST\n\n");
    driveRoad(WEST, 50);
    Exit();
  }

  if (Fork() == 0) {
    Delay(780);
    DPrintf("\n>> 6 arrives from WEST\n\n");
    driveRoad(WEST, 70);
    Exit();
  }

  if (Fork() == 0) {
    Delay(1200);
    DPrintf("\n>> 7 arrives from WEST\n\n");
    driveRoad(WEST, 60);
    Exit();
  }

  DPrintf("\n>> 1 arrives from WEST\n\n");
  driveRoad(WEST, 30);
}

// ***Test case 5 from discussion, Expected: 1,3,2
// 1 is VERY slow that 2 CANNOT take off, then 2 arrives
void test16() {
  DPrintf("TEST: From discussion, 1 arrives west but VERY slow. 2 arrives west too but CANNOT take off. Then 3 arrives east so it can go before 2\n");

  if (Fork() == 0) {
    Delay(100);
    DPrintf("\n>> 2 arrives from WEST\n\n");
    driveRoad(WEST, 100);
    Exit();
  }

  if (Fork() == 0) {
    Delay(120);
    DPrintf("\n>> 3 arrives from EAST\n\n");
    driveRoad(EAST, 100);
    Exit();
  }

  DPrintf("\n>> 1 arrives from WEST\n\n");
  driveRoad(WEST, 10); // Slow A!
}

// 1,2,3 closely arrive at west, but once D arrives before C, C should wait for D to finish.
void test17() {
  DPrintf("TEST: 1,2,3 closely arrive at west, but then 4 arrives at east, 4 should be able to go\n");

  if (Fork() == 0) {
    Delay(100);
    DPrintf("\n>> 2 arrives from WEST\n\n");
    driveRoad(WEST, 50);
    Exit();
  }

  if (Fork() == 0) {
    Delay(200);
    DPrintf("\n>> 3 arrives from WEST\n\n");
    driveRoad(WEST, 50);
    Exit();
  }

  if (Fork() == 0) {
    Delay(150);
    DPrintf("\n>> 4 arrives from EAST\n\n");
    driveRoad(EAST, 50);
    Exit();
  }

  DPrintf("\n>> 1 arrives from WEST\n\n");
  driveRoad(WEST, 50);
}

void Main()
{
  InitRoad();
  // test1();
  // test2();
  // test3();
  // test4();
  // test5();
  // test6();
  // test7();
  // test8();
  // test9();
  // test10();
  // test11();
  // test12();
  // test13();
  // test14();
  // test15();
  // test16();
  // test17();
  /* The following code is specific to this simulation,  e.g., number
   * of cars, directions and speeds. You should experiment with
   * different numbers of cars,  directions and speeds to test your
   * modification of driveRoad.  When your solution is tested, we
   * will use different Main procedures,  which will first call
   * InitRoad before any calls to driveRoad.  So,  you should do
   * any initializations in InitRoad. 
   */

  	if (Fork() == 0) {
      Delay(1200);			// car 2
      driveRoad(WEST, 60);
      Exit();
      }

      if (Fork() == 0) {
      Delay(900);			// car 3
      driveRoad(EAST, 50);
      Exit();
      }

      if (Fork() == 0) {
      Delay(900);			// car 4
      driveRoad(WEST, 30);
      Exit();
      }

      driveRoad(EAST, 40);			// car 1

      Exit();
}

/* Our tests will call your versions of InitRoad and driveRoad, so your
 * solution to the car simulation should be limited to modifying the code
 * below. This is in addition to your implementation of semaphores
 * contained in mycode3.c.  
 */

void InitRoad()
{
  /* do any initializations here */
  Regshm((char *) &shm, sizeof(shm));

  shm.east_wait = Seminit(0);
  shm.east_wait_cnt = 0;
  shm.west_wait = Seminit(0);
  shm.east_wait_cnt =0;
  shm.road_dir = -1;
  shm.cars_on_road =0;
  shm.mutex = Seminit(1);
  for(int i =1; i<NUMPOS+1; i++) {
    shm.pos[i]=0;
    shm.sempos[i]=Seminit(1);
  } 

}

#define IPOS(FROM)	(((FROM) == WEST) ? 1 : NUMPOS)

void driveRoad(int from, int mph)
  // from: coming from which direction
  // mph: speed of car
{
  int c;				// car ID c = process ID
  int p, np, i;			// positions

  c = Getpid();			// learn this car's ID
  Wait(shm.mutex);
  if(shm.road_dir==-1) shm.road_dir=from;
  if(from==WEST) {
    shm.west_wait_cnt ++;
  }else {
    shm.east_wait_cnt ++;
  }
  if (shm.cars_on_road>0 && shm.road_dir!= from) {
    if (from==WEST) {
      // shm.west_wait_cnt ++;
      Signal(shm.mutex);
      Wait(shm.west_wait);
      Wait(shm.mutex);
    } else if (from==EAST){
      // shm.east_wait_cnt ++;
      Signal(shm.mutex);
      Wait (shm.east_wait);
      Wait(shm.mutex);
    }
  }else if (shm.cars_on_road>0 && shm.road_dir == from) {
    if (from==WEST && (shm.pos[1]==1 || shm.east_wait_cnt>0) ) {
      // shm.west_wait_cnt ++;
      Signal(shm.mutex);
      Wait(shm.west_wait);
      Wait(shm.mutex);
    } else if (from == EAST && (shm.pos[NUMPOS]==1|| shm.west_wait_cnt>0)){
      // shm.east_wait_cnt ++;
      Signal(shm.mutex);
      Wait (shm.east_wait);
      Wait(shm.mutex);
    }
  }
  if(from==EAST){
    shm.cars_on_road++;
    shm.road_dir=from;
    shm.pos[NUMPOS]=1;
    shm.east_wait_cnt --;
    Signal(shm.mutex);
    Wait(shm.sempos[NUMPOS]);
    Wait(shm.mutex);
  }else if (from==WEST) {
    shm.pos[1]=1; 
    shm.cars_on_road++;
    shm.road_dir=from;
    shm.west_wait_cnt --;
    Signal(shm.mutex);
    Wait (shm.sempos[1]);
    Wait(shm.mutex);
  }

  // DO NOT MODIFY THE NEXT 3 STATEMENTS IN ANY WAY!
  EnterRoad(from);
  PrintRoad();
  Printf("Car %d enters at %d at %d mph\n", c, IPOS(from), mph);
  Signal(shm.mutex);
  for (i = 1; i < NUMPOS; i++) {

    if (from == WEST) {
      p = i;
      np = i + 1;
    } else {
      p = NUMPOS + 1 - i;
      np = p - 1;
    }
    // DO NOT MODIFY THE NEXT 4 STATEMENTS IN ANY WAY!
    Delay(3600/mph);
    Wait(shm.sempos[np]);
    ProceedRoad();
    Signal(shm.sempos[p]);
    Wait(shm.mutex);  
    if (i==1){
      if (from == EAST && shm.west_wait_cnt==0 && shm.east_wait_cnt>0) {
        Signal(shm.mutex);
        Signal(shm.east_wait);
        Wait(shm.mutex);
      }else if (from == WEST && shm.east_wait_cnt==0 && shm.west_wait_cnt>0) {
        // DPrintf("here");
        Signal(shm.mutex);
        Signal(shm.west_wait);
        Wait(shm.mutex);
      } 
    }
    shm.pos[np]=1;
    shm.pos[p]=0;
    Signal(shm.mutex);
    PrintRoad();
    Printf("Car %d moves from %d to %d\n", c, p, np);
  }

  // DO NOT MODIFY THE NEXT 4 STATEMENTS IN ANY WAY!
  Delay(3600/mph);
  ProceedRoad();
  PrintRoad();
  Printf("Car %d exits road\n", c);
  Signal(shm.sempos[np]);
  Wait(shm.mutex);
  shm.pos[np]=0;
  shm.cars_on_road --;
  if(from == WEST) {
    if (shm.cars_on_road ==0 && shm.east_wait_cnt>0 ) {
      shm.road_dir=EAST;
      Signal(shm.mutex);
      Signal(shm.east_wait);
    }else {
      Signal(shm.mutex);
    } 
  }else if (from == EAST){
    if (shm.cars_on_road ==0 && shm.west_wait_cnt>0 ) {
      shm.road_dir=WEST;
      Signal(shm.mutex);
      Signal(shm.west_wait);
    }else {
      Signal(shm.mutex);
    } 
  }else {
    Signal(shm.mutex);
  }
}
