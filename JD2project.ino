#include <motors.h>
#include <bluetooth.h>

//current angles in degrees
float current_angle1 = 90;
float current_angle2 = 90;

void rotate(String gcode, float angle1, float angle2) {
  //calculate the difference in angles
  float difference_1 = current_angle1 - angle1;
  float difference_2 = current_angle2 - angle2;

  //rotate the motors
  if (difference_1 != 0) {
    //enable the motors
    digitalWrite(EN1, LOW);
    rotateDegrees(STEP1, DIR1, difference_1 > 0, abs(difference_1), microsteppingMode1);
    current_angle1 = angle1;
    delayMicroseconds(50);
    digitalWrite(EN1, HIGH);
  };
  if (difference_2 != 0) {
    digitalWrite(EN2, LOW);
    rotateDegrees(STEP2, DIR2, difference_2 > 0, abs(difference_2), microsteppingMode2);
    current_angle2 = angle2;
    delayMicroseconds(50);
    digitalWrite(EN2, HIGH);
  }
};

void run() {
  Serial.println("=== Running Commands ===");
  for (int i = 0; i < commandCount; i++) {
    Serial.print("Executing ");
    Serial.print(commandList[i].gcode);
    Serial.print(": a1=");
    Serial.print(commandList[i].angle1, 6);
    Serial.print(", a2=");
    Serial.println(commandList[i].angle2, 6);

    rotate(commandList[i].gcode, commandList[i].angle1, commandList[i].angle2);
  }

  commandCount = 0;
  dataReady = false;
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32_BLE");

  setupPins();
  microstepping(1, 1);

  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService* pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  pServer->getAdvertising()->start();

  Serial.println("BLE Server started. Waiting for data...");
}

void loop() {
  if (dataReady) {
    run();
  }
}
