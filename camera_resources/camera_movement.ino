#include <Servo.h>

Servo x_base, y_base;

int x_axis = A0;
int y_axis = A1;
int val_x, val_y;

// boundaries of camera rotation in x and y axis
const int xMIN = 0, xMAX = 180, yMIN = 0, yMAX = 180;

class camDegree {
public:
  // Accessors
  void setX(int _x_) { x = _x_; }
  void setY(int _y_) { y = _y_; }
  int getX() { return x; }
  int getY() { return y; }

  // Public Functions
  void reset() {
    setX(90);
    setY(90);
  }
  void incX() { x = (x < xMAX) ? x + 1 : x; }
  void decX() { x = (x > xMIN) ? x - 1 : x; }
  void incY() { y = (y < yMAX) ? y + 1 : y; }
  void decY() { y = (y > yMIN) ? y - 1 : y; }
private:
  int x, y;
} eyeAngle;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  x_base.attach(9);
  y_base.attach(10);

  eyeAngle.reset();
}

void loop() {
  //put your main code here, to run repeatedly:
  
  val_x = analogRead(x_axis);
  if (val_x > 640 ) { eyeAngle.incX(); }
  else if (val_x < 384) { eyeAngle.decX(); }
  x_base.write(eyeAngle.getX());

  val_y = analogRead(y_axis);
  if (val_y > 640) { eyeAngle.incY(); }
  else if (val_y < 384) { eyeAngle.decY(); }
  y_base.write(eyeAngle.getY());

  Serial.print("X: "); Serial.print(eyeAngle.getX());
  Serial.print("\tY: "); Serial.println(eyeAngle.getY());
}

