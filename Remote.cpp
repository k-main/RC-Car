#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
// joystick buttons

const int autonomous = 5;
int autonomousFlag = 0;
int hold_auto = 0;

// button to shut down
const int shutDown = 21;
int shutDownFlag = 0;
int hold_shutdown = 0;

//buttons to move camera left and right
const int left = 12;
const int right = 13;   
int moveCamFlag = 0;


// button to save coordinates
const int saveCoordinates = 22;
int i = 0;
int saveCoordinatesFlag = 0;
float coordinates [10][2] = {
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

#define TFT_MISO   19
#define TFT_SCK    18
#define TFT_MOSI   23
#define TFT_DC      2
#define TFT_RESET   4
#define TFT_CS     15

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RESET, TFT_MISO); 

// write state machines here
enum joy_States { joy_SMStart, move_staight_forward, move_staight_backward, move_left_forward, move_left_backward, move_right_forward, move_right_backward, autonomous_mode} joy_State;


// analog pin 32 connected to X output of JoyStick
// analog pin 33 connected to Y output of JoyStick

int readStick() { 
  if (analogRead(33) < 200) {
    //going forward
    if(analogRead(32) < 200){
      //turn wheels left
      return 3;
    }
    else if(analogRead(32) > 3000){
      //turn wheels right
      return 5;
    }
    return 1; // forward and straight
  }
  else if(analogRead(33) > 3000){
    if(analogRead(32) < 200){
      //turn wheels left
      return 4;
    }
    else if(analogRead(32) > 3000){
      //turn wheels right
      return 6;
    }
    return 2; //backward and straight
  }
  else{
    return 0;
  }
  
}

void writeToDisplaySetup(){
  tft.fillRect(65,45, 200,150, ILI9341_BLACK);
  tft.setCursor(100,120);
}

// unsigned int joyStick = 0;

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
        joy_State = static_cast<joy_States>(readStick());
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
        joy_State = static_cast<joy_States>(readStick());
      }
    case move_staight_backward:
      if (shutDownFlag == 1){
        joy_State = joy_SMStart;
      }
      else if(autonomousFlag == 1){
        joy_State = autonomous_mode;
      }
      else{
        joy_State = static_cast<joy_States>(readStick());
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
        joy_State = static_cast<joy_States>(readStick());
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
        joy_State = static_cast<joy_States>(readStick());
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
        joy_State = static_cast<joy_States>(readStick());
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
        joy_State = static_cast<joy_States>(readStick());
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
      writeToDisplaySetup();
      tft.println("Stopping!"); 
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
        writeToDisplaySetup();
        tft.println("Going Straight & Forward!"); 
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
        writeToDisplaySetup();
        tft.println("Going Straight & Backward!"); 
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
        writeToDisplaySetup();
        tft.println("Going Left & Forward!"); 
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
        writeToDisplaySetup();
        tft.println("Going Left & Backward!");
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
        writeToDisplaySetup();
        tft.println("Going Right & Forward!"); 
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
        writeToDisplaySetup();
        tft.println("Going Right & Backward!");
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
        writeToDisplaySetup();
        tft.println("Autonomous");
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
// enum autonomous_States {autonomous_wait, autonomous_start} autonomous_State;

enum camera_States {camera_SMStart, move_left_180, move_right_180}camera_State;


void move_Camera() {
  switch (camera_State) { //Transitions
    case camera_SMStart:
      if((digitalRead(right) == HIGH) && (digitalRead(left) == LOW) && (moveCamFlag == 0)){
        moveCamFlag = 1;
        camera_State = move_right_180;
      }
      else if((digitalRead(left) == HIGH) && (digitalRead(right) == LOW) && (moveCamFlag == 0)){
        moveCamFlag = 1;
        camera_State = move_left_180;
      }
      else{
        camera_State = camera_SMStart;
      }
      break;
    case move_left_180:
      camera_State = camera_SMStart;
    
    case move_right_180:
      camera_State = camera_SMStart;

    default:
         camera_State = camera_SMStart;
         break; 
  }
  switch (camera_State){ //State Actions
    case camera_SMStart:
      if((digitalRead(right) == LOW) && (digitalRead(left) == LOW) && (moveCamFlag == 1)){
        moveCamFlag = 0;
      }
      break;
    case move_left_180:
      // send signal to move camera left
      break;
    case move_right_180:
      // send signal to move camera right
      break;  
    default:
         break; 
  }
}

enum coordinates_States {wait, save} coordinates_State;

void save_Coordinates() {
  switch (coordinates_State) { //Transitions
    case wait:
      if((digitalRead(saveCoordinates) == HIGH) && (saveCoordinatesFlag == 0)){
        coordinates_State = save;
      }
      else {
        coordinates_State = wait;
      }
      break;
    case save:
      coordinates_State = wait;

    default:
         coordinates_State = wait;
         break; 
  }
  switch (coordinates_State){ //State Actions
    case wait:
      if((digitalRead(saveCoordinates) == LOW) && (saveCoordinatesFlag == 1)){
        saveCoordinatesFlag = 0;
      }
      break;
    case save:
      saveCoordinatesFlag = 1;
      coordinates[i][0] = 5; // should be numbers that we get from gps
      coordinates[i][1] = 2;
      if (i < 10){
        i += 1;
      }
      else{
        i = 0;
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
  pinMode(right, INPUT);
  pinMode(left, INPUT);
  tft.begin();
  tft.setRotation(1);
  Serial.begin(115200);
  tft.fillRect(65,45, 200,150, ILI9341_BLACK);
  tft.setCursor(160,45);
  tft.setTextColor(tft.color565(255,255,255));
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  move_Car();
  delay(500);
  save_Coordinates();
  delay(500);
  move_Camera();
  // Serial.print("Shut Down: ");
  // Serial.println(digitalRead(shutDown));
  // Serial.print("Autonomous: ");
  // Serial.println(digitalRead(autonomous));
  // Serial.print("Save Coordinates: ");
  // Serial.println(digitalRead(saveCoordinates));
}
