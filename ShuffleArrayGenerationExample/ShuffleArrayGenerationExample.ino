#include <arduino.h>
#define L1 13
#define L2 12
#define L3 11
#define L4 10
#define B1 1
#define B2 2
#define B3 3
#define B4 4
#define LENGTH 4
#define SHUFFLE_MULT 5

void setup() {
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
  pinMode(L4, OUTPUT);
  pinMode(B1, INPUT);
  pinMode(B2, INPUT);
  pinMode(B3, INPUT);
  pinMode(B4, INPUT);
  
  randomSeed(analogRead(A0));  // Fixed randomSeed initialization
}

void loop() {
  // Implementazione e shuffle di un'array di size LENGTH
  int array1[LENGTH];
  for (int i = 0; i < LENGTH; i++) {
    array1[i] = i + 1;
  }
  for (int i = 0; i < LENGTH * SHUFFLE_MULT; i++) {
    int rand1 = random(LENGTH);
    int rand2 = random(LENGTH);
    int element1 = array1[rand1];
    int element2 = array1[rand2];
    array1[rand1] = element2;
    array1[rand2] = element1;
  }
  // Clear all LEDs before starting a new cycle
  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
  digitalWrite(L3, LOW);
  digitalWrite(L4, LOW);

  for (int i = 0; i < LENGTH; i++) {
    if (array1[i] == 1) {
      digitalWrite(L1, HIGH);
    }
    if (array1[i] == 2) {
      digitalWrite(L2, HIGH);
    }
    if (array1[i] == 3) {
      digitalWrite(L3, HIGH);
    }
    if (array1[i] == 4) {
      digitalWrite(L4, HIGH);
    }
    delay(1000);  // Wait for a second before moving to the next LED
  }
  delay(3000);
}
