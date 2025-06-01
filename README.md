# Junior Design II Project

## Project Description

This project is a Bluetooth-enabled 2-axis SCARA (Selective Compliance Articulated Robot Arm) designed to draw on standard 8.5" x 11" paper. Created by Team 018, the arm recieves gcode from a computer via Bluetooth and replicates them on paper using a mounted pen.

The system integrates precise motor control, kinematics, and embedded communication to bridge the digital and physical worlds—allowing users to “print” their digital sketches in real time. The SCARA configuration was selected for its accuracy, speed, and suitability for planar motion tasks such as plotting or handwriting.

> [!Warning]
> The code in this repository will certainly fail without the proper dependancies. See the installation section for details on the dependancies.

## Installation

The arduino code has several dependancies and you must install the libraries for the code to function correctly. To install the arduino libraries open the library manager in the arduino ide and install the following three libraries. After installing the libraries select the correct board from the boards manager. This may be different depending on the board you use. My project is using the ESP32 by NodeMCU so my chosen board is the NodeMCU-32S. 

> [!Note]
> The code takes roughly 90% of space on the ESP32 and takes quite a while to upload. Don't be worried if it takes a while to flash.

- ESP32Servo
  - By: Kevin Harrington, John K. Bennett
- ESP32 BLE Arduino
  - By: Neil Kolban, chegewara
- AccelStepper
  - By: Mike McCauley

The Python script depends on the bleak library, which manages the Bluetooth Low Energy (BLE) communication between the computer and the ESP32. You can install bleak using pip:

```
pip install bleak
```
