#include <Wire.h>
#include "Adafruit_VL53L0X.h"
#include "Arduino.h"


Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);  // Higher baud rate recommended for ESP32
  //pinMode(12,OUTPUT);
  // Initialize I2C for ESP32
  Wire.begin(21, 22);
  Serial.println("Adafruit VL53L0X test.");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1) delay(10);
  }
  Serial.println(F("VL53L0X API Continuous Ranging example\n\n"));
  // Start continuous ranging
  lox.startRangeContinuous();
}

void loop() {
  //get the distance
  Serial.print("Radius: ");
  int radius = lidarScan();
  Serial.println(radius);
  //delay(100); //this causes some slightly better accuracy
  
}
int lidarScan(){ // is there a way to get this more accurate?
  int dist = lox.readRange();
  return dist;
}