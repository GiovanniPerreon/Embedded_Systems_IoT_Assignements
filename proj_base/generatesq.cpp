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

bool isEqual(int a1[], int a2[]) {
  for (int i = 0; i < SQLENGTH; i++) {
    if (a1[i] != a2[i]) {
      return false;
    }
  }
  return true;
}

bool isPresent(int array[], int i) {
  for (int j = 0; j < SQLENGTH; j++) {
    if (array[j] == i) {
      return true;
    }
  }
  return false;
}
