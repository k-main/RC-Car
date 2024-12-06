#include <Arduino.h>
#include <tasks.h>
#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"


#define NUM_TASKS 3

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
#define HB0_B0 21
#define HB0_B1 22
#define HB1_B0 17
#define HB1_B1 16

//d21 d22 d17(tx2) d16(rx2)

const unsigned int GCD_PERIOD = 1;
const unsigned int IN_PERIOD  = 50;
const unsigned int M_PERIOD   = 1;
const unsigned int NRF_PERIOD = 100;

task tasks[NUM_TASKS];

typedef enum switch_state {high, low};
typedef enum input_state {wait, read_btn};
typedef enum m_state {left, right, off, forward, back};
m_state car_dir;
m_state rotation_dir;
m_state motor_state;
 
double ON_X = 0, ON_Y = 0, PWM_PERIOD = 16;
short PWM_X = 0, PWM_Y = 0;

void updateTasks(void);
int tick_in(int state);
int tick_m(int state);
int tick_nrf(int state);

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
    tasks[i].taskFunc = &tick_m;
    i++;
    tasks[i].period = IN_PERIOD;
    tasks[i].state = wait;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskFunc = &tick_in;
    i++;
    tasks[i].period = NRF_PERIOD;
    tasks[i].state = read_btn;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].taskFunc = &tick_nrf;
    i++;
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

int tick_nrf(int state) {
  if (radio.available()){
    radio.read(&RECV_BUFFER, sizeof(RECV_BUFFER));
    int motor_ctrl = (int)RECV_BUFFER[0];
    int jst_x = (int)RECV_BUFFER[1];
    int jst_y = (int)RECV_BUFFER[2];
    // switch(motor_ctrl){
    //   case 0:
    //     Serial.print("Motors off.");
    //     car_dir = off;
    //     break;
    //   case 1:
    //     Serial.print("Backwards.");
    //     car_dir = left;
    //     break;
    //   case 2:
    //     Serial.print("Forward.");
    //     car_dir = right;
    //     break;
    // }

    if (jst_x > 16) {
      rotation_dir = right;
      ON_X = jst_x;
    } else if (jst_x < 14) {
      rotation_dir = left;
      ON_X = 31 - jst_x;
    } else {
      rotation_dir = off;
    }

    if (jst_y > 16) {
      car_dir = right;
      ON_Y = jst_y;
    } else if (jst_y < 12) {
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

    Serial.print("ON Y: ");
    Serial.println(ON_Y);

    memset(&RECV_BUFFER, 0, sizeof(RECV_BUFFER));
  }


  return state;
}

int tick_in(int state) {
  short pin32, pin33;
  switch(state){
    case wait:
      // if (digitalRead(TEST_BTN1) > 0 || digitalRead(TEST_BTN2) > 0) {
      //   state = read_btn;
      //   break;
      // } else {
      //   state = wait;
      //   car_dir = off;
      // }
    break;
    case read_btn:
      // pin32 = digitalRead(TEST_BTN1);
      // pin33 = digitalRead(TEST_BTN2);

      // if (pin32 == 1) {
      //   if (pin33 == 1){
      //     car_dir = off;
      //     state = wait;
      //     break;
      //   } else {
      //     car_dir = right;
      //     state = read_btn;
      //     break;
      //   }
      // } else if (pin33 == 1) {
      //   car_dir = left;
      //   state = read_btn;
      //   break;
      // } else {
      //   state = wait;
      //   car_dir = off;
      // }
    break;
  }

  return state;
}

int tick_m(int state) {
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
      if (PWM_X <= ON_X) {
        m_left();
      } else {
        m_off();
      }
      break; 
      case right:
      PWM_X = (PWM_X > 31) ? 0 : PWM_X + 1;;
      if (PWM_X <= ON_X) {
        m_right();
      } else {
        m_off();
      }
      break;
      m_off();
    }

  }

  return state;
}