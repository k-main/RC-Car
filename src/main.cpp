#include <Arduino.h>
#include "timerISR.h"
#include "helper.h"
#include "periph.h"

//TODO: declare variables for cross-task communication

/* You have 5 tasks to implement for this lab */
#define NUM_TASKS 2


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


int tick_in(int state);
int tick_m(int state);


int main(void) {
    motor_state = off;
    unsigned char i = 0;
    serial_init(9600);
    DDRD = 0b0000;
    DDRC = 0b1111;
    PORTC = 0b0101;

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

inline void m_off(void){
  PORTC = 0b0101;
}

inline void m_right(void){
  PORTC = 0b0101;
  PORTC = 0b1100;
}

inline void m_left(void){
  PORTC = 0b0101;
  PORTC = 0b0011;
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