#include <esp32-hal-gpio.h>
#include <Arduino.h>
#include <ESP32Servo.h>

// Define pin connections stepper motor 1
#define DIR1 23
#define STEP1 22
#define MS11 18
#define MS12 19   //motor one, pin 2
#define MS13 21
#define EN1 5

//Define pin connections stepper motor 2
#define DIR2 25
#define STEP2 26
#define MS21 12
#define MS22 14   //motor twos, pin 2
#define MS23 27
#define EN2 13

//Define Servo connections
#define SERV 17

//motor's steps per revolution
const int stepsPerRevolution = 200;
Servo myServo; //servo object
int microsteppingMode1 = 1;
int microsteppingMode2 = 1;


void microstepping(int mode1, int mode2) {
  switch (mode1) {
    case 1:
      digitalWrite(MS11, LOW); digitalWrite(MS12, LOW); digitalWrite(MS13, LOW); break;
    case 2:
      digitalWrite(MS11, HIGH); digitalWrite(MS12, LOW); digitalWrite(MS13, LOW); break;
    case 4:
      digitalWrite(MS11, LOW); digitalWrite(MS12, HIGH); digitalWrite(MS13, LOW); break;
    case 8:
      digitalWrite(MS11, HIGH); digitalWrite(MS12, HIGH); digitalWrite(MS13, LOW); break;
    case 16:
      digitalWrite(MS11, HIGH); digitalWrite(MS12, HIGH); digitalWrite(MS13, HIGH); break;
  }
  switch (mode2) {
    case 1:
      digitalWrite(MS21, LOW); digitalWrite(MS22, LOW); digitalWrite(MS23, LOW); break;
    case 2:
      digitalWrite(MS21, HIGH); digitalWrite(MS22, LOW); digitalWrite(MS23, LOW); break;
    case 4:
      digitalWrite(MS21, LOW); digitalWrite(MS22, HIGH); digitalWrite(MS23, LOW); break;
    case 8:
      digitalWrite(MS21, HIGH); digitalWrite(MS22, HIGH); digitalWrite(MS23, LOW); break;
    case 16:
      digitalWrite(MS21, HIGH); digitalWrite(MS22, HIGH); digitalWrite(MS23, HIGH); break;
  }
  microsteppingMode1 = mode1;
  microsteppingMode2 = mode2;
}

void setupPins() {
  // Stepper Motor 1
  pinMode(DIR1, OUTPUT);
  pinMode(STEP1, OUTPUT);
  pinMode(MS11, OUTPUT);
  pinMode(MS12, OUTPUT);
  pinMode(MS13, OUTPUT);
  pinMode(EN1, OUTPUT);

  // Stepper Motor 2
  pinMode(DIR2, OUTPUT);
  pinMode(STEP2, OUTPUT);
  pinMode(MS21, OUTPUT);
  pinMode(MS22, OUTPUT);
  pinMode(MS23, OUTPUT);
  pinMode(EN2, OUTPUT);

  // Servo
  pinMode(SERV, OUTPUT);
}

void rotateDegrees(int stepPin, int dirPin, bool clockwise, float degrees, int mode) {
  float stepsPerDegree = (stepsPerRevolution * mode) / 360;
  int stepCount = round(degrees * stepsPerDegree);

  digitalWrite(dirPin, clockwise ? HIGH : LOW);

  for (int i = 0; i < stepCount; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800);  // speed tuning
    digitalWrite(stepPin, LOW);
    delayMicroseconds(800);
  }
}
