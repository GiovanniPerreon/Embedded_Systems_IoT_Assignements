#ifndef __CORE__
#define __CORE__

#define INTRO_STATE   1
#define STAGE1_STATE  2
#define STAGE2_STATE  3
#define STAGE3_STATE  4
#define FINAL_STATE   5
/* core business logic  */

void initCore();
void intro();
void stage1();
void stage2();
void stage3();
void finalize();
int getDifficulty();

#endif
