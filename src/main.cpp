#include <Arduino.h>
#include <tasks.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

#define NUM_TASKS 2

#define BTN_1 32
#define BTN_2 33
// #define LED_1 33
#define CE    4
#define CSN   5
#define SCK   18
#define MOSI  23
#define MISO  19

const unsigned int GCD_PERIOD = 50;
const unsigned int BTN_PERIOD = 50; 
const unsigned int NRF_PERIOD = 100;

task tasks[NUM_TASKS];

typedef enum btn_state { press, wait };
char fwd_flag = 0;
char bwd_flag = 0;
// const char* MSG_BUFFER = "Hello from ESP32.";
char MSG_BUFFER[16];
const byte  PIPE_ADDR[6] = "00001";
RF24 radio(4, 5);

void updateTasks(void);
int btn_func(int state);
int nrf_func(int state);


void setup() {
  // pinMode(LED_1, OUTPUT);
  pinMode(BTN_1, INPUT);
  pinMode(BTN_2, INPUT);

  radio.begin();
  radio.openWritingPipe(PIPE_ADDR);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  Serial.begin(115200);
  unsigned int i = 0;
  tasks[i].elapsedTime = 0;
  tasks[i].period = 50;
  tasks[i].state = wait;
  tasks[i].taskFunc = &btn_func;
  i++;
  tasks[i].elapsedTime = 0;
  tasks[i].period = 50;
  tasks[i].state = wait;
  tasks[i].taskFunc = &nrf_func;
  i++;
}

void loop() {
  delay(GCD_PERIOD);
  updateTasks();
}

int nrf_func(int state){
  if (fwd_flag) {
    MSG_BUFFER[0] = 'F';
  } else if (bwd_flag) {
    MSG_BUFFER[0] = 'B';
  } else {
    MSG_BUFFER[0] = 'N';
  }

  radio.write(&MSG_BUFFER, sizeof(MSG_BUFFER));
  return state;
}

int btn_func(int state){
  switch(state){
    case wait:

      if (digitalRead(BTN_1) > 0 || digitalRead(BTN_2) > 0) {
        state = press;
      } else {
        state = wait;
      }

    break;
    case press:

      if (digitalRead(BTN_1) < 1 && digitalRead(BTN_2) < 1) {
        state = wait;
        bwd_flag = fwd_flag = 0;
      } else {
        state = press;
        if (digitalRead(BTN_1)) {
          Serial.println("Right button.");
          bwd_flag = 0;
          fwd_flag = 1;
        }
        if (digitalRead(BTN_2)) {
          Serial.println("Left button.");
          fwd_flag = 0;
          bwd_flag = 1;
        }
      }

    break;
  }
  return state;
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