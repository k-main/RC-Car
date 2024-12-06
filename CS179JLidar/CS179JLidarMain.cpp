//Note this is now version 3, I did not properly commit the changes proeprly at the time. As of this push it is one big push.
//Version 4 update, no longer able to use stepper motor with esp32, had to switch to arduino for stepper motor itself
//Version 5 update, no longer using the stepper motor, nor the adafruit library, as I figured out how I2C communication works
//Version 6 update, now using the servo with automatic "crash detection"
//Version 7 update, I2C communication is now properly setup, error bounds are temporarily setup and next will be the drawing

// References 
// https://randomnerdtutorials.com/esp32-pwm-arduino-ide/
// https://www.st.com/resource/en/datasheet/vl53l0x.pdf
// https://randomnerdtutorials.com/esp32-i2c-master-slave-arduino/
// https://docs.arduino.cc/learn/communication/wire/
// 

#include <Arduino.h>
#include <Wire.h>

// Pins
#define servo1 4
#define servo2 5
#define offWire 23

// TOF sensor addresses
#define VL53L0X_ADDRESS 0x29  // Default address
#define VL53L0X_ADDRESS2 0x30 // New address for the second sensor

// Servo variables
#define servoMinPW 500
#define servoMaxPW 2500
#define servoPeriod 20000
#define PWMFreq 50
#define PWMRes 16
#define PWMChan1 0
#define PWMChan2 1

//Setting up servos you must first establish a PWM channel, then you must
// configure that channel to your servo pin to allow PWM control
void setUpServos() {
    ledcSetup(PWMChan1, PWMFreq, PWMRes);
    ledcAttachPin(servo1, PWMChan1);

    ledcSetup(PWMChan2, PWMFreq, PWMRes);
    ledcAttachPin(servo2, PWMChan2);
}

//Setting this up was not fun, but calculating the pulse width so that servo
// does not go past 180, and finding the documentation for the minimum pulse width
// to the maximum pulse width was difficult. Duty cycle was just plug and play. Had
// to make arrangements for which pwm channel to use, so code is cleaner and more usable.
void setServoAngle(int angle, int pwmType) {
    int pulseWidth = map(angle, 0, 180, servoMinPW, servoMaxPW);
    uint32_t dutyCycle = (pulseWidth * ((1 << PWMRes) - 1)) / servoPeriod;

    if (pwmType == 1){
        ledcWrite(PWMChan1, dutyCycle);
    } 
    if (pwmType == 2){
        ledcWrite(PWMChan2, dutyCycle);
    }
}

//This iteration actually functions consistently, turns out wire.h
// has a decent amount of functionality, and I was thinking about the setting of
// addresses incorrectly, this makes it so I can setup multiple differnet sensors if necessary
void setupTOF(TwoWire &wire, int oldAddress, int newAddress) {
    wire.beginTransmission(oldAddress);
    wire.write(0x8A);  // Address register
    wire.write(newAddress);
    wire.endTransmission();
    delay(10);
}

//This readDistance function has not changed until now, turns out that due
// wire being an object I can just read the distance as I normally would, just
// passing in a different object. The distance register was taken from the adafruit
// documentation (best documentation ever)
int readDistance(TwoWire &wire, int address) {
    wire.beginTransmission(address);
    wire.write(0x14 + 10);  // Read distance register, some error offset
    wire.endTransmission(false);

    wire.requestFrom(address, 2);
    if (wire.available() == 2) {
        int distance = wire.read() << 8 | wire.read();
        return distance;
    }
    return -1; // In case the wire is not available or something unplugged
}
//Setts up getters/setters to make life easier for i2c devices
void setRegister(TwoWire &wire, int address, int reg, int value) {
    wire.beginTransmission(address);
    wire.write(reg);
    wire.write(value);
    wire.endTransmission();
}

// Function to get a register value
int getRegister(TwoWire &wire, int address, int reg) {
    wire.beginTransmission(address);
    wire.write(reg);
    wire.endTransmission(false);
    wire.requestFrom(address, 1);
    return Wire.read();
}
void setup() {
    Serial.begin(115200);

    setUpServos();
    setServoAngle(180, 1);
    setServoAngle(180, 2);

    // Setup I2C for the first sensor
    Wire.begin(18, 19); // SDA = 18, SCL = 19
    setupTOF(Wire, VL53L0X_ADDRESS, VL53L0X_ADDRESS); //Initialize sensor 1

    // Setup I2C for the second sensor
    Wire1.begin(21, 22); // SDA = 21, SCL = 22
    setupTOF(Wire1, VL53L0X_ADDRESS, VL53L0X_ADDRESS2); //Initialize sensor 2
}

//Gets angles from 0-180 outputs the angles, then gets the distance at that time
// if the distance is too small it will not be able to initiate the automatic breaking
// ideally if the distance is under 200 or ove 1000, it will not be printed
//Still doing testing on the wifi stuff, documentation is quite nice
void loop() {
    setRegister(Wire,VL53L0X_ADDRESS, 0x00, 0x02);  // Start continuous ranging for the first sensor
    setRegister(Wire1,VL53L0X_ADDRESS2, 0x00, 0x02); // Start continuous ranging for the second sensor
    for (int i = 0; i < 180; i+=3) {
        setServoAngle(180-i, 1);
        setServoAngle(180 - i, 2);
        //
        int distance1 = readDistance(Wire, VL53L0X_ADDRESS);
        int distance2 = readDistance(Wire1, VL53L0X_ADDRESS2);

        // Filter out values below 200 mm or above 1000 mm
            Serial.print("Angle1: ");
            Serial.print(i);
            Serial.print(" | Distance1: ");
            Serial.print(distance1);
            Serial.print(" Angle2: ");
            Serial.print(i+180);
            Serial.print(" | Distance2: ");
            Serial.println(distance2);
 

        delay(100); // Reduce delay for more frequent updates
    }
    delay(10000);
    for(int i = 179; i >= 0; i-=3){
        setServoAngle(180-i, 1);
        setServoAngle(180 - i, 2);
        int distance1 = readDistance(Wire, VL53L0X_ADDRESS);
        int distance2 = readDistance(Wire1, VL53L0X_ADDRESS2);
        // Filter out values below 200 mm or above 1000 mm
            Serial.print("Angle1: ");
            Serial.print(i);
            Serial.print(" | Distance1: ");
            Serial.print(distance1);
            Serial.print(" Angle2: ");
            Serial.print(i+180);
            Serial.print(" | Distance2: ");
            Serial.println(distance2);
 

        delay(100); // Reduce delay for more frequent updates
    }
    delay(10000);
}
// for any values that are below 200mm or above 1000 mm delete them