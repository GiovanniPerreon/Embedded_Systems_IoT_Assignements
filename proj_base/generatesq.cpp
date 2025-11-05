#include "generatesq.h"
#include "arduino.h"
#include "config.h"

int generateNum(int array[], int i) {
  int num = random(SQLENGTH) + 1;
  for (int j = 0; j < i; j++) {
    if (array[j] == num) {
      num = generateNum(array, i);
    }
  }
  return num;
}

void generate(int array[]) {
  for (int i = 0; i < SQLENGTH; i++) {
    array[i] = generateNum(array, i);
  }
}
