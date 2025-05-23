#include "HardwareSerial.h"
#include "Print.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLECharacteristic* pCharacteristic;
bool dataReady = false;
String receivedLabel;
String receivedData;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

struct ArmCommand {
  String gcode;
  float angle1;
  float angle2;
};

const int MAX_COMMANDS = 100;
ArmCommand commandList[MAX_COMMANDS];
int commandCount = 0;


class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Client connected");
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Client disconnected, restarting advertising...");
    dataReady = true;
    delay(500); // Optional: small delay helps avoid advertising glitches
    pServer->getAdvertising()->start();
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    ArmCommand cmd;
    String value = pCharacteristic->getValue();

    //Serial.print("Received: ");
    //Serial.println(value.c_str());

    size_t colonPos = value.indexOf(":");
    receivedLabel = value.substring(0, colonPos);
    receivedData = value.substring(colonPos + 1);
    //Serial.print("Label: ");
    //Serial.println(receivedLabel.c_str());
    //Serial.print("Data: ");
    //Serial.println(receivedData.c_str());
    
    //Rips gcode from value and stores it in cmd.
    int gStart = value.indexOf("'gcode': '") + 10;
    int gEnd = value.indexOf("'", gStart);
    cmd.gcode = value.substring(gStart, gEnd);
    //Serial.println(cmd.gcode);

    //Rips angle2 from value and stores it in cmd
    int twoStart = value.indexOf("'a2': ") + 6;
    int twoEnd = value.indexOf(",", twoStart);
    String two = value.substring(twoStart, twoEnd);
    //Serial.println(two);
    cmd.angle2 = two.toFloat();
    //Serial.println(cmd.angle2, 9);

    //Rips angle1 from value and stores it in cmd
    int oneStart = value.indexOf("'a1': ") + 6;
    int oneEnd = value.indexOf("}", oneStart);
    String one = value.substring(oneStart, oneEnd);
    //Serial.println(one);
    cmd.angle1 = one.toFloat();
    //Serial.println(cmd.angle1, 9);

    commandList[commandCount] = cmd;
    commandCount++;

    // Example: respond with the label
    pCharacteristic->setValue(receivedLabel);
    pCharacteristic->notify();
  }
};