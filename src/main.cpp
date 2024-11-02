#include <Arduino.h>
#include "timerISR.h"
#include "helper.h"
#include "periph.h"

#define PCH_Q1 8
#define NCH_Q2 11
#define PCH_Q3 10
#define NCH_Q4 9

//TODO: declare variables for cross-task communication

/* You have 5 tasks to implement for this lab */
#define NUM_TASKS 3


//Task struct for concurrent synchSMs implmentations
typedef struct _task{
	signed 	 char state; 		//Task's current state
	unsigned long period; 		//Task period
	unsigned long elapsedTime; 	//Time elapsed since last task tick
	int (*TickFct)(int); 		//Task tick function
} task;


//TODO: Define Periods for each task


// e.g. const unsined long TASK1_PERIOD = <PERIOD>
const unsigned int GCD_PERIOD = 50;
const unsigned int SW_PERIOD = 500;
const unsigned int IN_PERIOD = 50;
const unsigned int M_PERIOD = 50;
typedef enum switch_state {high, low};
typedef enum input_state {wait, read};
typedef enum m_state {left, right, off};
m_state motor_state;


task tasks[NUM_TASKS];

void TimerISR() {
    
    //TODO: sample inputs here

	for ( unsigned int i = 0; i < NUM_TASKS; i++ ) {                   // Iterate through each task in the task array
		if ( tasks[i].elapsedTime == tasks[i].period ) {           // Check if the task is ready to tick
			tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
			tasks[i].elapsedTime = 0;                          // Reset the elapsed time for the next tick
		}
		tasks[i].elapsedTime += GCD_PERIOD;                        // Increment the elapsed time by GCD_PERIOD
	}
}

int tick_in(int state) {
  short pin3, pin4;
  switch(state){
    case wait:
      if (digitalRead(3) > 0 || digitalRead(4) > 0) {
        state = read;
        break;
      } else {
        state = wait;
        motor_state = off;
      }
    break;
    case read:
      pin3 = digitalRead(3);
      pin4 = digitalRead(4);

      if (pin3 == 1) {
        if (pin4 == 1){
          motor_state = off;
          state = wait;
          break;
        } else {
          motor_state = right;
          state = read;
          break;
        }
      } else if (pin4 == 1) {
        motor_state = left;
        state = read;
        break;
      } else {
        state = wait;
        motor_state = off;
      }
    break;
  }

  return state;
}

void m_off(void){
  digitalWrite(PCH_Q1, HIGH);
  digitalWrite(PCH_Q3, HIGH);
  digitalWrite(NCH_Q2, LOW);
  digitalWrite(NCH_Q4, LOW);
}

void m_right(void){
  digitalWrite(PCH_Q1, LOW);
  digitalWrite(NCH_Q4, HIGH);
}

void m_left(void){
  digitalWrite(PCH_Q3, LOW);
  digitalWrite(NCH_Q2, HIGH);
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

int tick_sw(int state) {
  switch(state){
    case high:
      digitalWrite(2, HIGH);
      state = low;
      break;
    case low:
      // state = high;
      digitalWrite(2, LOW);
      break;
  }
  return state;
}

int main(void) {
    //TODO: initialize all your inputs and ouputs

/*
To initialize a pin as an output, you must set its DDR value as ‘1’ and then set its PORT value as a ‘0’. 
To initialize a pin as an input, you do the opposite: you must set its DDR value as ‘0’ and its PORT value as a ‘1’. 
*/

  /*
  #define PCH_Q1 8
#define NCH_Q2 9
#define PCH_Q3 10
#define PCH_Q4 11
  
  */
    motor_state = off;
    unsigned char i = 0;
    serial_init(9600);
    // DDRC = 0x34; PORTC = 0x33;
    // DDRB = 0b111110; PORTB = 0b111101;
    // DDRD = 0xFF; PORTD = 0x00;
    // DDRD = 0b0010000000000; PORTD = 0b0000; 
    pinMode(2,  OUTPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);

    pinMode(PCH_Q1, OUTPUT);
    pinMode(NCH_Q2, OUTPUT);
    pinMode(PCH_Q3, OUTPUT);
    pinMode(NCH_Q4, OUTPUT);


    

    // ADC_init();   // initializes ADC
    //TODO: Initialize tasks here
    // tasks[i].period = display_period;
    // tasks[i].state = display_cm;
    // tasks[i].elapsedTime = tasks[i].period;
    // tasks[i].TickFct = &tick_displayfct;
    // i++;
    
    tasks[i].period = SW_PERIOD;
    tasks[i].state = low;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &tick_sw;
    i++;
    tasks[i].period = M_PERIOD;
    tasks[i].state = low;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &tick_m;
    i++;
    tasks[i].period = IN_PERIOD;
    tasks[i].state = wait;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &tick_in;
    i++;
    TimerSet(GCD_PERIOD);
    TimerOn();
    while(1){

    }

    return 0;
}
