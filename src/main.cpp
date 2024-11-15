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
#define PMOS_01 21
#define NMOS_02 22
#define PMOS_03 17
#define NMOS_04 16

#define TEST_BTN1 32
#define TEST_BTN2 33

//d21 d22 d17(tx2) d16(rx2)

const unsigned int GCD_PERIOD = 50;
const unsigned int IN_PERIOD  = 50;
const unsigned int M_PERIOD   = 50;
const unsigned int NRF_PERIOD = 1000;

task tasks[NUM_TASKS];

typedef enum switch_state {high, low};
typedef enum input_state {wait, read_btn};
typedef enum m_state {left, right, off};
m_state motor_state;
 
void updateTasks(void);
int tick_in(int state);
int tick_m(int state);
int tick_nrf(int state);

inline void m_off(void){
  digitalWrite(PMOS_01, HIGH);
  digitalWrite(PMOS_03, HIGH);
  digitalWrite(NMOS_02, LOW);
  digitalWrite(NMOS_04, LOW);
}

inline void m_right(void);
inline void m_left(void);

void setup() {
    motor_state = off;
    unsigned char i = 0;
    Serial.begin(115200);
    // DDRC = 0b1111;
    // PORTC = 0b0101;
    pinMode(PMOS_01, OUTPUT);
    pinMode(NMOS_02, OUTPUT);
    pinMode(PMOS_03, OUTPUT);
    pinMode(NMOS_04, OUTPUT);

    pinMode(TEST_BTN1, INPUT);
    pinMode(TEST_BTN2, INPUT);

    m_off();

    radio.begin();
    radio.openReadingPipe(0,PIPE_ADDR);
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();

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
  m_off();
  digitalWrite(PMOS_01, LOW);
  digitalWrite(NMOS_04, HIGH);
  // PORTC = 0b0101;
  // digitalWrite(PMOS_03, HIGH);
  // digitalWrite(NMOS_02, LOW);
  // PORTC = 0b1100;
}

inline void m_left(void){
  m_off();
  digitalWrite(NMOS_02, HIGH);
  digitalWrite(PMOS_03, LOW);
  // PORTC = 0b0101;
  // PORTC = 0b0011;
  // digitalWrite(PMOS_01, HIGH);
  // digitalWrite(NMOS_04, LOW);
}

int tick_nrf(int state) {
  if (radio.available()){
    radio.read(&RECV_BUFFER, sizeof(RECV_BUFFER));
    Serial.println((const char*)RECV_BUFFER);
    memset(&RECV_BUFFER, 0, sizeof(RECV_BUFFER));
  }


  return state;
}

int tick_in(int state) {
  short pin32, pin33;
  switch(state){
    case wait:
      if (digitalRead(TEST_BTN1) > 0 || digitalRead(TEST_BTN2) > 0) {
        state = read_btn;
        break;
      } else {
        state = wait;
        motor_state = off;
      }
    break;
    case read_btn:
      pin32 = digitalRead(TEST_BTN1);
      pin33 = digitalRead(TEST_BTN2);

      if (pin32 == 1) {
        if (pin33 == 1){
          motor_state = off;
          state = wait;
          break;
        } else {
          motor_state = right;
          state = read_btn;
          break;
        }
      } else if (pin33 == 1) {
        motor_state = left;
        state = read_btn;
        break;
      } else {
        state = wait;
        motor_state = off;
      }
    break;
  }

  return state;
}

int tick_m(int state) {
  switch(motor_state){
    case left:
      m_left();
    break;
    case right:
      m_right();
    break;
    case off:
      m_off();
    break;
  }

  return state;
}