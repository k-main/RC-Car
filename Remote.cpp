#include <Arduino.h>
// joystick buttons

const int autonomous = 5;
int autonomousFlag = 0;
int hold_auto = 0;

// button to shut down
const int shutDown = 21;
int shutDownFlag = 0;
int hold_shutdown = 0;


// button to save coordinates
const int saveCoordinates = 22;
int i = 0;
int saveCoordinatesFlag = 0;
int coordinates [10][2] = {
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0},
  {0,0}
}; 

// analog pin 32 connected to X output of JoyStick
// analog pin 33 connected to Y output of JoyStick

int readStick() { //returns 1 if the joystick was up, 2 if it is down, 0 for anything else
  // you may have to read from A0 instead of A1 depending on how you orient the joystick
  if (analogRead(33) < 200) {
    //going froward
    if(analogRead(32) < 200){
      //turn wheels left
      return 0;
    }
    else if(analogRead(32) > 3000){
      //turn wheels right
      return 1;
    }
    return 2; // forward and straight
  }
  else if(analogRead(33) > 3000){
    if(analogRead(32) < 200){
      //turn wheels left
      return 3;
    }
    else if(analogRead(32) > 3000){
      //turn wheels right
      return 4;
    }
    return 5; //backward and straight
  }
  else{
    return 6;
  }
  
}

unsigned int joyStick = 0;

// write state machines here
enum joy_States { joy_SMStart, move_staight_forward, move_staight_backward, move_left_forward, move_left_backward, move_right_forward, move_right_backward, autonomous_mode} joy_State;

void move_Car() {
  switch (joy_State) { //Transitions
    case joy_SMStart:
      if (shutDownFlag == 1){
        joy_State = joy_SMStart;
      }
      else if(autonomousFlag == 1){
        joy_State = autonomous_mode;
      }
      else{
        if (joyStick == 0){
          joy_State = move_left_forward;
        }
        else if (joyStick == 1){
          joy_State = move_right_forward;
        }
        else if(joyStick == 2){
          joy_State = move_staight_forward;
        }
        else if (joyStick == 3){
          joy_State = move_left_backward;
        }
        else if (joyStick == 4){
          joy_State = move_right_backward;
        }
        else if (joyStick == 5){
          joy_State = move_staight_backward;
        }
        else{
          joy_State = joy_SMStart;
        }
      }
 
      break;
    case move_staight_forward:
      if (shutDownFlag == 1){
        joy_State = joy_SMStart;
      }
      else if(autonomousFlag == 1){
        joy_State = autonomous_mode;
      }
      else{
        if (joyStick == 0){
          joy_State = move_left_forward;
        }
        else if (joyStick == 1){
          joy_State = move_right_forward;
        }
        else if(joyStick == 2){
          joy_State = move_staight_forward;
        }
        else if (joyStick == 3){
          joy_State = move_left_backward;
        }
        else if (joyStick == 4){
          joy_State = move_right_backward;
        }
        else if (joyStick == 5){
          joy_State = move_staight_backward;
        }
        else{
          joy_State = joy_SMStart;
        }
      }
    case move_staight_backward:
      if (shutDownFlag == 1){
        joy_State = joy_SMStart;
      }
      else if(autonomousFlag == 1){
        joy_State = autonomous_mode;
      }
      else{
        if (joyStick == 0){
          joy_State = move_left_forward;
        }
        else if (joyStick == 1){
          joy_State = move_right_forward;
        }
        else if(joyStick == 2){
          joy_State = move_staight_forward;
        }
        else if (joyStick == 3){
          joy_State = move_left_backward;
        }
        else if (joyStick == 4){
          joy_State = move_right_backward;
        }
        else if (joyStick == 5){
          joy_State = move_staight_backward;
        }
        else{
          joy_State = joy_SMStart;
        }
      }
    case move_left_forward:
        //move gears
        //turn Wheels left
      if (shutDownFlag == 1){
        joy_State = joy_SMStart;
      }
      else if(autonomousFlag == 1){
        joy_State = autonomous_mode;
      }
      else{
        if (joyStick == 0){
          joy_State = move_left_forward;
        }
        else if (joyStick == 1){
          joy_State = move_right_forward;
        }
        else if(joyStick == 2){
          joy_State = move_staight_forward;
        }
        else if (joyStick == 3){
          joy_State = move_left_backward;
        }
        else if (joyStick == 4){
          joy_State = move_right_backward;
        }
        else if (joyStick == 5){
          joy_State = move_staight_backward;
        }
        else{
          joy_State = joy_SMStart;
        }
      }
    case move_left_backward:
        //move gears opposite way
        //turn Wheels left
      if (shutDownFlag == 1){
        joy_State = joy_SMStart;
      }
      else if(autonomousFlag == 1){
        joy_State = autonomous_mode;
      }
      else{
        if (joyStick == 0){
          joy_State = move_left_forward;
        }
        else if (joyStick == 1){
          joy_State = move_right_forward;
        }
        else if(joyStick == 2){
          joy_State = move_staight_forward;
        }
        else if (joyStick == 3){
          joy_State = move_left_backward;
        }
        else if (joyStick == 4){
          joy_State = move_right_backward;
        }
        else if (joyStick == 5){
          joy_State = move_staight_backward;
        }
        else{
          joy_State = joy_SMStart;
        }
      }
    case move_right_forward:
        //move gears
        //turn Wheels right
      if (shutDownFlag == 1){
        joy_State = joy_SMStart;
      }
      else if(autonomousFlag == 1){
        joy_State = autonomous_mode;
      }
      else{
        if (joyStick == 0){
          joy_State = move_left_forward;
        }
        else if (joyStick == 1){
          joy_State = move_right_forward;
        }
        else if(joyStick == 2){
          joy_State = move_staight_forward;
        }
        else if (joyStick == 3){
          joy_State = move_left_backward;
        }
        else if (joyStick == 4){
          joy_State = move_right_backward;
        }
        else if (joyStick == 5){
          joy_State = move_staight_backward;
        }
        else{
          joy_State = joy_SMStart;
        }
      }
    case move_right_backward:
        //move gears opposite way
        //turn Wheels left  
      if (shutDownFlag == 1){
        joy_State = joy_SMStart;
      }
      else if(autonomousFlag == 1){
        joy_State = autonomous_mode;
      }
      else{
        if (joyStick == 0){
          joy_State = move_left_forward;
        }
        else if (joyStick == 1){
          joy_State = move_right_forward;
        }
        else if(joyStick == 2){
          joy_State = move_staight_forward;
        }
        else if (joyStick == 3){
          joy_State = move_left_backward;
        }
        else if (joyStick == 4){
          joy_State = move_right_backward;
        }
        else if (joyStick == 5){
          joy_State = move_staight_backward;
        }
        else{
          joy_State = joy_SMStart;
        }
      }
      case autonomous_mode:
        //car moves automatically 
      if(shutDownFlag == 1){
        joy_State = joy_SMStart;
      } 
      else if(autonomousFlag == 1){
        joy_State = autonomous_mode;
      }
      else{
        joy_State = joy_SMStart;
      }
    default:
         joy_State = joy_SMStart;
         break; 
  }
  switch (joy_State){ //State Actions
    case joy_SMStart:
      // stop car
      // Serial.println("Stopping!"); 
      joyStick = readStick();
      if((digitalRead(shutDown) == HIGH)){
          if(hold_shutdown == 0 && shutDownFlag == 1){
            hold_shutdown = 1;
            shutDownFlag = 0;
          }
          else if(hold_shutdown == 0 && shutDownFlag == 0){
            hold_shutdown = 1;
            shutDownFlag = 1;
          }
      }
      if((digitalRead(shutDown) == HIGH)){
          hold_shutdown = 0;
      }
      if(digitalRead(autonomous) == HIGH){
        hold_auto = 0;
      }
      else if(digitalRead(autonomous) == LOW && hold_auto == 0){
          hold_auto = 1;
          autonomousFlag = 1;
      }
      break;
    case move_staight_forward:
        //move gears
        //keep Wheels Straight
        // Serial.println("Going Straight & Forward!"); 
        joyStick = readStick();
        if(digitalRead(shutDown) == HIGH){
          hold_shutdown = 1;
          shutDownFlag = 1;
        }
        else if(digitalRead(autonomous) == LOW && hold_auto == 0){
          hold_auto = 1;
          autonomousFlag = 1;
        }
        break;
    case move_staight_backward:
        //move gears opossite way
        //keep Wheels Straight
        // Serial.println("Going Straight & Backward!"); 
        joyStick = readStick();
        if(digitalRead(shutDown) == HIGH){
          hold_shutdown = 1;
          shutDownFlag = 1;
        }
        else if(digitalRead(autonomous) == LOW && hold_auto == 0){
          hold_auto = 1;
          autonomousFlag = 1;
        }
        break;
    case move_left_forward:
        // Serial.println("Going Left & Forward!"); 
        joyStick = readStick();
        if(digitalRead(shutDown) == HIGH){
          hold_shutdown = 1;
          shutDownFlag = 1;
        }
        else if(digitalRead(autonomous) == LOW && hold_auto == 0){
          hold_auto = 1;
          autonomousFlag = 1;
        }
        //move gears
        //turn Wheels left
        break;
    case move_left_backward:
        // Serial.println("Going Left & Backward!");
        joyStick = readStick();
        if(digitalRead(shutDown) == HIGH){
          hold_shutdown = 1;
          shutDownFlag = 1;
        }
        else if(digitalRead(autonomous) == LOW && hold_auto == 0){
          hold_auto = 1;
          autonomousFlag = 1;
        }
        //move gears opposite way
        //turn Wheels left
        break;
    case move_right_forward:
        // Serial.println("Going Right & Forward!"); 
        joyStick = readStick();
        if(digitalRead(shutDown) == HIGH){
          hold_shutdown = 1;
          shutDownFlag = 1;
        }
        else if(digitalRead(autonomous) == LOW && hold_auto == 0){
          hold_auto = 1;
          autonomousFlag = 1;
        }
        //move gears
        //turn Wheels right
        break;
    case move_right_backward:
        // Serial.println("Going Right & Backward!");
        joyStick = readStick();
        if(digitalRead(shutDown) == HIGH){
          hold_shutdown = 1;
          shutDownFlag = 1;
        }
        else if(digitalRead(autonomous) == LOW && hold_auto == 0){
          hold_auto = 1;
          autonomousFlag = 1;
        }
        //move gears opposite way
        //turn Wheels left  
        break; 
    case autonomous_mode:
        // Serial.println("Autonomous");
        if(digitalRead(shutDown) == HIGH){
          hold_shutdown = 1;
          shutDownFlag = 1;
        }
        else if((digitalRead(autonomous) == LOW)){
          if(hold_auto == 0 && autonomousFlag == 1){
            hold_auto = 1;
            autonomousFlag = 0;
          }
        }
        else if((digitalRead(autonomous) == HIGH)){
          hold_auto = 0;
        }
        break;
    default:
         break; 
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(shutDown, INPUT);
  pinMode(saveCoordinates, INPUT);
  pinMode(autonomous, INPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  move_Car();
  if((digitalRead(saveCoordinates) == HIGH) && (saveCoordinatesFlag == 0)){
    saveCoordinatesFlag = 1;
    coordinates[i][0] = 5;
    coordinates[i][1] = 2;
    if (i < 10){
      i += 1;
    }
    else{
      i = 0;
    }
    // Serial.println("done!");
  }
  if((digitalRead(saveCoordinates) == LOW) && (saveCoordinatesFlag == 1)){
    saveCoordinatesFlag = 0;
  }
  // Serial.print("Shut Down: ");
  // Serial.println(digitalRead(shutDown));
  // Serial.print("Autonomous: ");
  // Serial.println(digitalRead(autonomous));
  // Serial.print("Save Coordinates: ");
  // Serial.println(digitalRead(saveCoordinates));
   
}