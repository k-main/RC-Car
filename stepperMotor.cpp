#incldue "Arduino.h"
#define dirPin 4
#define stepPin 3
#define enablePin 2
#define MS1 5
#define MS2 6
#define MS3 7
#define interruptPin 8
const int stepDelay = 500;  // Adjust this value

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  // Set microstepping to 1/2
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, LOW);

  digitalWrite(enablePin, LOW);  // Enable the driver
  digitalWrite(dirPin, HIGH);   // Set direction
  pinMode(interruptPin,INPUT); //sets up hall sensor to detect 
}

void loop() {
  if(digitalRead(interruptPin == LOW)){
    digitalWrite(dirPin, HIGH); 
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
  else{ //This will let the stepper motor know to rotate if it did a 360
    digitalWrite(dirPin, LOW);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
}
