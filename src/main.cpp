#include <Arduino.h>
#include <tasks.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

#define NUM_TASKS 1

#define BTN_1 32
#define LED_1 33
#define CE    4
#define CSN   5
#define SCK   18
#define MOSI  23

const unsigned int GCD_PERIOD = 50;
const unsigned int BTN_PERIOD = 50; 
unsigned long timer = 0;

task tasks[NUM_TASKS];

typedef enum btn_state { press, wait };

void updateTasks(void);
int btn_func(int state);

void setup() {
  
  pinMode(LED_1, OUTPUT);
  pinMode(BTN_1, INPUT);

  Serial.begin(115200);
  unsigned int i = 0;
  tasks[i].elapsedTime = 0;
  tasks[i].period = 50;
  tasks[i].state = wait;
  tasks[i].taskFunc = &btn_func;
  i++;
}

void loop() {
  delay(GCD_PERIOD);
  updateTasks();
}

int btn_func(int state){
  switch(state){
    case wait:

      if (digitalRead(BTN_1) > 0) {
        state = press;
        digitalWrite(LED_1, HIGH);
      } else {
        state = wait;
      }

    break;
    case press:

      if (digitalRead(BTN_1) < 1) {
        state = wait;
        digitalWrite(LED_1, LOW);
      } else {
        state = press;
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