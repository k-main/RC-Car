//Note this is now version 3, I did not properly commit the changes proeprly at the time. As of this push it is one big push.
//Version 4 update, no longer able to use stepper motor with esp32, had to switch to arduino for stepper motor itself
#include <Wire.h>
#include "Adafruit_VL53L0X.h"
#include "Arduino.h"


Adafruit_VL53L0X lox = Adafruit_VL53L0X();
#define interruptPin 15
#define outputPinArduino 5 // this will output a high signal to let the arduino know to turn the stepper motor around

//Vars to be set prior
float angle = 0; //beginning of the angle
// We will need two full rotation in order to make 360 degrees
// Each step angle is 1.8 degrees an it takes 200 steps for full rotation
// 360/(200 * 2) * 2 = 1.8 degress/loop
float stepAngle = 1.8;
volatile int lastPinState = 0;
volatile bool beginAngle = false;

//ISR
void IRAM_ATTR ISR_hallFunction(){ // might need to test on its own first
  int curState = digitalRead(interruptPin);
  if(curState == HIGH){
    if(lastPinState == 0){
      lastPinState = 1;
    }
  }
  else if(curState == LOW){
    if(lastPinState == 1){
      lastPinState = 0;
      beginAngle = true;
      digitalWrite(outputPinArduino, HIGH);
      delay(100); // wait for the arduino to turn the stepper motor around
      digitalWrite(outputPinArduino, LOW); // turns the output pin off after the arduino has turned the stepper motor around
    }
  }
}

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
  pinMode(interruptPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), ISR_hallFunction, CHANGE); //should send interrupt
  pinMode(outputPinArduino, OUTPUT);
  digitalWrite(outputPinArduino, LOW); //initializes the output pin
}

void loop() {
  // setup some way to reset angle with hall sensor
  if(beginAngle == true){ // When magnet is detected, we reset the angle
    angle = 0;
    beginAngle == false;
  }
  // get the distance
  Serial.print("Radius: ");
  int radius = lidarScan();
  Serial.println(radius);
  Serial.print(", Angle: ");
  Serial.println(angle);
  angle = angle + stepAngle;
  //delay(100); //this causes some slightly better accuracy
  
}
int lidarScan(){ // is there a way to get this more accurate?
  int dist = lox.readRange();
  return dist;
}

