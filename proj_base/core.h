#ifndef __CORE__
#define __CORE__

#define INTRO_STATE   1
#define STAGE1_STATE  2
#define STAGE2_STATE  3
#define STAGE3_STATE  4
#define STAGE4_STATE  5
#define FINAL_STATE   6 
/* core business logic  */

void initCore();
void intro();
void stage1();
void stage2();
void stage3();
void stage4();
void finalize();
int getDifficulty();

#endif
