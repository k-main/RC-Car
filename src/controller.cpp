#include <Arduino.h>
#include <tasks.h>
#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "Wire.h"
#include "MPU6050.h"


#define NUM_TASKS 2

/* NRF24L01 Module SPI Pins & Setup */
#define CE    4
#define CSN   5
#define SCK   18
#define MOSI  23
#define MISO  19

RF24 radio(CE, CSN);
const byte PIPE_ADDR[6] = "00001";
char RECV_BUFFER[16] = "";

/* Motor Signal Output Pins */
#define HB0_B0 33
#define HB0_B1 32
#define HB1_B0 17
#define HB1_B1 16

//d21 d22 d17(tx2) d16(rx2)

#define MPU_SDA 21
#define MPU_SCL 22

const unsigned int GCD_PERIOD = 1;
const unsigned int ACCEL_PERIOD = 200;
const unsigned int M_PERIOD   = 1;
const unsigned int NRF_PERIOD = 20;

task tasks[NUM_TASKS];

typedef enum switch_state {high, low};
typedef enum input_state {wait, read_btn};
typedef enum m_state {left, right, off, forward, back};
m_state car_dir;
m_state rotation_dir;
m_state motor_state;
double ON_X = 0, ON_Y = 0, PWM_PERIOD = 16;
short PWM_X = 0, PWM_Y = 0;
 

typedef enum sensor {read_val};
short accel_read_cnt = 0, accel_read_max = 3;
int32_t sumAx = 0, sumAy = 0, sumAz = 0;
int32_t sumGx = 0, sumGy = 0, sumGz = 0;
int16_t ax, ay, az, gx, gy, gz;
MPU6050 mpu;

void updateTasks(void);
int in_func(int state);
int motor_func(int state);
int nrf_func(int state);
int accel_func(int state);

inline void m_off(void){
  if (motor_state == off) return;
  digitalWrite(HB1_B0, LOW);
  digitalWrite(HB1_B1, LOW);
  digitalWrite(HB0_B0, LOW);
  digitalWrite(HB0_B1, LOW);



  motor_state = off;
}

inline void m_fwd(void);
inline void m_bwd(void);
inline void m_left(void);
inline void m_right(void);

void setup() {
    car_dir = off;
    rotation_dir = off;
    unsigned char i = 0;
    Serial.begin(115200);

    Wire.begin(MPU_SDA, MPU_SCL);
    mpu.initialize();
    if (mpu.testConnection()) {
      Serial.println("MPU6050 connection successful");

      // automatic calibration (leave the unit standing still until 5 seconds)
      Serial.println("Calibrating gyroscope...");
      mpu.CalibrateGyro(6);
      Serial.println("Calibration complete.");
      mpu.PrintActiveOffsets();
    } else {
      Serial.println("MPU6050 initialization failed.");
      // while(1);
    }

    radio.begin();
    radio.openReadingPipe(0,PIPE_ADDR);
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();

    pinMode(HB0_B0, OUTPUT);
    pinMode(HB0_B1, OUTPUT);
    pinMode(HB1_B0, OUTPUT);
    pinMode(HB1_B1, OUTPUT);

    motor_state = off;
    m_off();

    Serial.println("NRF24L initialized successfully.");

    tasks[i].period = M_PERIOD;
    tasks[i].state = low;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskFunc = &motor_func;
    i++;
    tasks[i].period = NRF_PERIOD;
    tasks[i].state = read_btn;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskFunc = &nrf_func;
    i++;
    // tasks[i].period = ACCEL_PERIOD;
    // tasks[i].state = read_val;
    // tasks[i].elapsedTime = tasks[i].period;
    // tasks[i].taskFunc = &accel_func;
    // i++;
}


void loop() {
  delay(GCD_PERIOD);
  updateTasks();
}


void updateTasks(void){
  for (unsigned int i = 0; i < NUM_TASKS; i++) {
    tasks[i].elapsedTime += GCD_PERIOD;
    if (tasks[i].elapsedTime >= tasks[i].period) {
      tasks[i].state = tasks[i].taskFunc(tasks[i].state);
      tasks[i].elapsedTime = 0;
    }
  }
}


inline void m_right(void){
  if (motor_state == right) return;
  digitalWrite(HB1_B0, LOW);
  digitalWrite(HB1_B1, HIGH);
  digitalWrite(HB0_B0, LOW);
  digitalWrite(HB0_B1, HIGH);
  motor_state = right;
}

inline void m_left(void){
  if (motor_state == left) return;
  digitalWrite(HB1_B0, HIGH);
  digitalWrite(HB1_B1, LOW);
  digitalWrite(HB0_B0, HIGH);
  digitalWrite(HB0_B1, LOW);
  motor_state = left;
}

inline void m_fwd(void){
  if (motor_state == forward) return;
  digitalWrite(HB1_B0, LOW);
  digitalWrite(HB1_B1, HIGH);

  digitalWrite(HB0_B0, HIGH);
  digitalWrite(HB0_B1, LOW);
  motor_state = forward;
}

inline void m_bwd(void){
  if (motor_state == back) return;
  digitalWrite(HB1_B0, HIGH);
  digitalWrite(HB1_B1, LOW);

  digitalWrite(HB0_B0, LOW);
  digitalWrite(HB0_B1, HIGH);
  motor_state = back;
}

int accel_func(int state) {
  for (char i = 0; i < accel_read_max; i++) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    sumAx += ax / accel_read_max;
    sumAy += ay / accel_read_max;
    sumAz += az / accel_read_max;
    sumGx += gx / accel_read_max;
    sumGy += gy / accel_read_max;
    sumGz += gy / accel_read_max;
  }
  
  // Serial.print("MPU6050: ");
  // Serial.print(" Acc[X]="); Serial.print(sumAx);
  // Serial.print(" Acc[Y]="); Serial.print(sumAy);
  // Serial.print(" Acc[Z]="); Serial.print(sumAz);
  // Serial.print(" Gyro[X]="); Serial.print(sumGx);
  // Serial.print(" Gyro[Y]="); Serial.print(sumGy);
  // Serial.print(" Gyro[Z]="); Serial.println(sumGz);

  return state;
}

int nrf_func(int state) {
  if (radio.available()){
    radio.read(&RECV_BUFFER, sizeof(RECV_BUFFER));
    int motor_ctrl = (int)RECV_BUFFER[0];
    int jst_x = (int)RECV_BUFFER[1];
    int jst_y = (int)RECV_BUFFER[2];

    if (jst_x > 17) {
      rotation_dir = right;
      ON_X = jst_x;
    } else if (jst_x < 13) {
      rotation_dir = left;
      ON_X = 31 - jst_x;
    } else {
      rotation_dir = off;
    }

    if (jst_y > 17) {
      car_dir = right;
      ON_Y = jst_y;
    } else if (jst_y < 11) {
      car_dir = left;
      ON_Y = 31 - jst_y;
    } else {
      car_dir = off;
    }


    Serial.println(" ");

    Serial.print("Joystick X: ");
    Serial.println((int)RECV_BUFFER[1]);
    Serial.print("ON X: ");
    Serial.println(ON_X);

    Serial.print("Joystick Y: ");
    Serial.println((int)RECV_BUFFER[2]);

    // Serial.print("ON Y: ");
    // Serial.println(ON_Y);

    memset(&RECV_BUFFER, 0, sizeof(RECV_BUFFER));
  }


  return state;
}

int motor_func(int state) {
  switch(car_dir){
    case left:
      PWM_Y = (PWM_Y > 31) ? 0 : PWM_Y + 1;
      if (PWM_Y <= ON_Y) {
      m_bwd();
      } else {
        m_off();
      }
    break;
    case right:
      PWM_Y = (PWM_Y > 31) ? 0 : PWM_Y + 1;
      if (PWM_Y <= ON_Y) {
      m_fwd();
      } else {
        m_off();
      }
    break;
    case off:
      PWM_Y = 0;
      m_off();
    break;
  }
  
  if (car_dir == off) {
    switch(rotation_dir){
      case left:
      PWM_X = (PWM_X > 31) ? 0 : PWM_X + 1;
      if (PWM_X <= ON_X) {
        m_left();
      } else {
        m_off();
      }
      break; 
      case right:
      PWM_X = (PWM_X > 31) ? 0 : PWM_X + 1;
      if (PWM_X <= ON_X) {
        m_right();
      } else {
        m_off();
      }
      break;
      case off:
      PWM_X = 0;
      m_off();
      break;
    }

  }

  return state;
}