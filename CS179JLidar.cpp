//Note this is now version 3, I did not properly commit the changes proeprly at the time. As of this push it is one big push.
//Version 4 update, no longer able to use stepper motor with esp32, had to switch to arduino for stepper motor itself
//Version 5 update, no longer using the stepper motor, nor the adafruit library, as I figured out how I2C communication works
//Version 6 update, now using the servo with automatic "crash detection"

// References 
// https://randomnerdtutorials.com/esp32-pwm-arduino-ide/
// https://www.st.com/resource/en/datasheet/vl53l0x.pdf
// https://randomnerdtutorials.com/esp32-i2c-master-slave-arduino/

#include <Arduino.h>
#include <Wire.h>

// Pins
#define servo1 4
#define servo2 5
#define offWire 23

// Vars needed
#define servoMinPW 500
#define servoMaxPW 2500
#define servoPeriod 20000
#define PWMFreq 50
#define PWMRes 16
#define PWMChan1 0
#define PWMChan2 1

// TOF sensor addresses
#define TOF_ADDRESS 0x29  // Default address
#define TOF_ADDRESS_2 0x30 // New address for the second sensor

#define TOF_RESULT 0x14  // Register where the result is stored
#define TOF_START 0x00       // Register that starts the measuring

void setUpServos() {
    // PWM channel setup, then attach the servo to the PWM channel
    // Sets up servo1
    ledcSetup(PWMChan1, PWMFreq, PWMRes);
    ledcAttachPin(servo1, PWMChan1);
    // Sets up servo2
    ledcSetup(PWMChan2, PWMFreq, PWMRes);
    ledcAttachPin(servo2, PWMChan2);
}

void setServoAngle(int angle, int pwmType) {
    int pulseWidth = map(angle, 0, 180, servoMinPW, servoMaxPW);
    uint32_t dutyCycle = (pulseWidth * ((1 << PWMRes) - 1)) / servoPeriod;
    // Write the duty cycle to the correct PWM channels
    if (pwmType == 1) {
        ledcWrite(PWMChan1, dutyCycle);
    } else if (pwmType == 2) {
        ledcWrite(PWMChan2, dutyCycle);
    }
}

// Function to set a register value
void setRegister(int address, int reg, int value) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

// Function to get a register value
int getRegister(int address, int reg) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(address, 1);
    return Wire.read();
}

void setupVL53L0X(int address) {
    setRegister(address, TOF_START, 0x01);  // Start regular mode
}

// Function to read distance from a specific sensor
int readDistance(int address) {
    Wire.beginTransmission(address);
    Wire.write(TOF_RESULT + 10);  // Offset for the result register
    Wire.endTransmission(false);
    Wire.requestFrom(address, 2);

    if (Wire.available() == 2) {
        int distance = Wire.read() << 8 | Wire.read(); // Combine high and low byte
        return distance;
    } else {
        return -1;  // Error reading distance
    }
}

void setup() {
    Serial.begin(115200); // Make sure baud rate is proper
    setUpServos();
    setServoAngle(0, 1);
    setServoAngle(180, 2);

    // Setup I2C
    Wire.begin(18, 19);

    // Initialize the first sensor and change its address
    setupVL53L0X(TOF_ADDRESS);
    setRegister(TOF_ADDRESS, 0x8A, TOF_ADDRESS_2); // Change address to 0x30

    // Initialize the second sensor with the default address
    delay(100);
    setupVL53L0X(TOF_ADDRESS);
    
    // Set the Wire to let the car no to stop
    pinMode(offWire, OUTPUT);
    digitalWrite(offWire, LOW);
}

void loop() {
    int angle1 = 180;
    int distance1 = 0;
    int distance2 = 0;

    setRegister(TOF_ADDRESS, 0x00, 0x02);  // Start continuous ranging for the first sensor
    setRegister(TOF_ADDRESS_2, 0x00, 0x02); // Start continuous ranging for the second sensor

    for (int i = 0; i < angle1; i++) {
        setServoAngle(i, 1);
        setServoAngle(180-i,2);
        distance1 = readDistance(TOF_ADDRESS);
        distance2 = readDistance(TOF_ADDRESS_2);
        if((distance1 < 400) || (distance2 < 400)){
          digitalWrite(offWire, HIGH);
          delay(1000);
          digitalWrite(offWire, LOW);
        }
        Serial.print("Angle1: ");
        Serial.print(i);
        Serial.print(" || Distance1: ");
        Serial.print(distance1);
        Serial.print(" || Angle2: ");
        Serial.print(180-i);
        Serial.print(" || Distance2: ");
        Serial.println(distance2);
        delay(10);
    }
    delay(1000);
    for (int i = 0; i < angle1; i++) {
        setServoAngle(180 - i, 1);
        setServoAngle(i, 2);
        distance1 = readDistance(TOF_ADDRESS);
        distance2 = readDistance(TOF_ADDRESS_2);
        if((distance1 < 400) || (distance2 < 400)){
          digitalWrite(offWire, HIGH);
          delay(1000);
          digitalWrite(offWire, LOW);
        }
        Serial.print("Angle1: ");
        Serial.print(180-i);
        Serial.print(" || Distance1: ");
        Serial.print(distance1);
        Serial.print(" || Angle2: ");
        Serial.print(i);
        Serial.print(" || Distance2: ");
        Serial.println(distance2);
        delay(10);
    }
    delay(10000);
}