#include <Wire.h>
#include "Adafruit_VL53L0X.h"
#include "Arduino.h"


Adafruit_VL53L0X lox = Adafruit_VL53L0X();

//Define some pins to be set for StepperMotor
#define dirPin 2 //need to change this
#define stepPin 4
#define enablePin 5

//Vars to be set prior
float angle = 0; //beginning of the angle
// We will need two full rotation in order to make 360 degrees
// Each step angle is 1.8 degrees an it takes 200 steps for full rotation
// 360/(200 * 2) * 2 = 1.8 degress/loop
float stepAngle = 1.8;
int stepDelay = 1000;
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
  // setup StepperMotor
  pinMode(dirPin,OUTPUT);
  pinMode(stepPin,OUTPUT);
  pinMode(enablePin,OUTPUT);
  digitalWrite(enablePin,LOW);
  digitalWrite(dirPin,HIGH);

}

void loop() {
  // setup some way to reset angle with hall sensor
  stepStepperMotor();
  //get the distance
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
void stepStepperMotor(){
  digitalWrite(stepPin,HIGH);
  delay(stepDelay); //need to check if we increase or not
  digitalWrite(stepPin,LOW);
  delay(stepDelay); //need to check if we increase or not
}
