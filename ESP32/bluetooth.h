#include <Arduino.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>

//GLOBAL VARIABLES//

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

//create two new accelstepper objects 
AccelStepper stepper1(AccelStepper::DRIVER, STEP1, DIR1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP2, DIR2);

//motor's steps per revolution
const int stepsPerRevolution = 200;
Servo myServo; //servo object

volatile bool paused = false;
volatile uint32_t lastInterrupt = 0;

#define PAUSESWITCH 4
#define BUILTIN_LED 2

void IRAM_ATTR handlePauseInterrupt() {
  uint32_t now = millis();
  if (now - lastInterrupt > 200) {
    paused = !paused;
    lastInterrupt = now;
    if (paused) {
      digitalWrite(BUILTIN_LED, HIGH);
    } else {
      digitalWrite(BUILTIN_LED, LOW);
    }
  }
}

void setupMicrostepping1(int mode) {
  Serial.println("IN SETUP M1");
  // mode: 1=full, 2=half, 4=quarter, 8=eighth, 16=sixteenth
  switch (mode) {
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
}

void setupMicrostepping2(int mode) {
  Serial.println("IN SETUP M2");
  // mode: 1=full, 2=half, 4=quarter, 8=eighth, 16=sixteenth
  switch (mode) {
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
}

void rotateDegrees(float degrees1, float degrees2, int microstepMode) {
  Serial.println("IN ROTATE DEGREES");
  int totalStepsPerRev = stepsPerRevolution * microstepMode;
  setupMicrostepping1(microstepMode);
  setupMicrostepping2(microstepMode);

  //Calculate how many steps needed
  float steps1 = (-1) * (degrees1 / 360) * totalStepsPerRev * 3;
  float steps2 = (degrees2 / 360) * totalStepsPerRev;

  stepper1.move(steps1);
  stepper2.move(steps2);

  // move the motors
  while(stepper1.distanceToGo() != 0 || stepper2.distanceToGo() != 0) {
    while (paused) {
      //Serial.println("IN FUCKING PAUSED");
      delay(10);
    }
    //Serial.println("IN WHILE LOOP");
    stepper1.run();
    stepper2.run();
  }

  
}

void setupPins() {
  pinMode(STEP1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(MS11, OUTPUT);
  pinMode(MS12, OUTPUT);
  pinMode(MS13, OUTPUT);
  pinMode(EN1, OUTPUT);

  pinMode(STEP2, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(MS21, OUTPUT);
  pinMode(MS22, OUTPUT);
  pinMode(MS23, OUTPUT);
  pinMode(EN2, OUTPUT);

  pinMode(SERV, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  myServo.attach(SERV);
}
