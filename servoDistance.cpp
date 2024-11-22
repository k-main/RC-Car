#include <Arduino.h>

#define SERVO_PIN 18 // Pin connected to the servo
#define SERVO_PIN2 19 // Pin connected to servo 2
#define PWM_FREQUENCY 50 // Servo PWM frequency in Hz
#define PWM_RESOLUTION 16 


#define SERVO_MIN_DUTY ((1.0 / 20.0) * pow(2, PWM_RESOLUTION)) // ~5%
#define SERVO_MAX_DUTY ((2.0 / 20.0) * pow(2, PWM_RESOLUTION)) // ~10%
void setServoAngle(int angle) {
  // Map the angle (0-180) to the duty cycle
  uint32_t duty = map(angle, 0, 180, SERVO_MIN_DUTY, SERVO_MAX_DUTY);
  // Write the PWM duty cycle
  ledcWrite(0, duty);
}
void setServoAngle2(int angle) {
  // Map the angle (0-180) to the duty cycle
  uint32_t duty = map(angle, 0, 180, SERVO_MIN_DUTY, SERVO_MAX_DUTY);
  // Write the PWM duty cycle
  ledcWrite(1, duty);
}
void setup() {

  ledcSetup(0, PWM_FREQUENCY, PWM_RESOLUTION); // Channel 0, 50Hz, 16-bit resolution
  ledcAttachPin(SERVO_PIN, 0);                 // Attach pin to PWM channel 0
  ledcSetup(1, PWM_FREQUENCY, PWM_RESOLUTION); // Channel 1, 50Hz, 16-bit resolution
  ledcAttachPin(SERVO_PIN2, 1);                 // Attach pin to PWM channel 1
}

void loop() {
  setServo1();
  setServo2();
}
void setServo1(){
    for (int angle = 0; angle <= 180; angle += 10) {
    setServoAngle(angle);
    //delay(500); // Wait for the servo to reach the position
  }
  for (int angle = 180; angle >= 0; angle -= 10) {
    setServoAngle(angle);
    //delay(500); // Wait for the servo to reach the position
  }
}
void setServo2(){ 
    for (int angle = 0; angle <= 180; angle += 10) {
    setServoAngle2(angle);
    //delay(500); // Wait for the servo to reach the position
  }
  for (int angle = 180; angle >= 0; angle -= 10) {
    setServoAngle2(angle);
    //delay(500); // Wait for the servo to reach the position
  }
}


