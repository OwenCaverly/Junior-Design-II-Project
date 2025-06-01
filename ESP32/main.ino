#include <motors.h>
#include <bluetooth.h>

//current angles in degrees
float current_angle1 = 0;
float current_angle2 = 0;

String gcode_prev;

void rotate(String gcode, float angle1, float angle2) {
  //calculate the difference in angles
  float difference_1 = angle1 - current_angle1;
  float difference_2 = angle2 - current_angle2;
  
  Serial.print("Angle1: ");
  Serial.print(difference_1);
  Serial.print(" Angle2: ");
  Serial.println(difference_2);

  //control whether the servo is up or down
  if (gcode == "G0" && gcode != gcode_prev) {
    //raise the servo
    myServo.write(0);
    delay(500);
  } else if (gcode == "G1" && gcode != gcode_prev) {
    //lower the servo
    myServo.write(100);
    delay(500);
  }
  
  gcode_prev = gcode;

  rotateDegrees(difference_1, difference_2, 16);
  current_angle1 = angle1;
  current_angle2 = angle2;
};

void return_home() {
  rotateDegrees(-current_angle1, -current_angle2, 16);
  current_angle1 = 0;
  current_angle2 = 0;
};

void run() {
  //Serial.println("=== Running Commands ===");
  for (int i = 0; i < commandCount; i++) {
    /*
    Serial.print("Executing ");
    Serial.print(commandList[i].gcode);
    Serial.print(": a1=");
    Serial.print(commandList[i].angle1, 6);
    Serial.print(", a2=");
    Serial.println(commandList[i].angle2, 6);
    */
    rotate(commandList[i].gcode, commandList[i].angle1, commandList[i].angle2);
  }

  commandCount = 0;
  dataReady = false;
};

void setup() {
  Serial.begin(115200);

  pinMode(PAUSESWITCH, INPUT);
  attachInterrupt(digitalPinToInterrupt(PAUSESWITCH), handlePauseInterrupt, CHANGE);

  BLEDevice::init("ESP32_BLE");
  setupPins();

  
  stepper1.setMaxSpeed(90000);
  stepper1.setAcceleration(60000);
  stepper2.setMaxSpeed(30000);
  stepper2.setAcceleration(20000); 
  

  /*
  stepper1.setMaxSpeed(6000);
  stepper1.setAcceleration(3000);
  stepper2.setMaxSpeed(2500);
  stepper2.setAcceleration(1333); 
  */

  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService* pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);

  pService->start();
  pServer->getAdvertising()->start();

  //Serial.println("BLE Server started. Waiting for data...");
}

void loop() {
  if (dataReady) {
    digitalWrite(EN1, LOW);
    digitalWrite(EN2, LOW);
    run();
    myServo.write(0);
    delay(50);
    return_home();
    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, HIGH);
  }
}
