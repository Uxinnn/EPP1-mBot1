#include "MeMCore.h"

// Motor variables
MeDCMotor motorL(M2);  // left motor
MeDCMotor motorR(M1);  // right motor
int speedL = 245;  // speed of left motor
int speedR = 255;  // speed of right motor
int turnDelayL = 335;  // in milliseconds, trial and error
int turnDelayR = 300;
int gridDelay = 1000;

//--------------CONTROLS--------------//
void halt(){
  motorL.stop();
  motorR.stop();
}

void  turn_right(){
  forward(-speedL,-speedR);
  delay(turnDelayR);
  halt();
}

void turn_left() {
  forward(speedL,speedR);
  delay(turnDelayL);
  halt();
}

void turn_around() {
  turn_right();
  halt();
  delay(10);
  turn_right();
}

void forward() {
  motorL.run(-speedL);
  motorR.run(speedR);
}

// overloading forward with 2 integer inputs
void forward(int left, int right) {
  motorL.run(left);
  motorR.run(right);
}

void successive_left_turn() {
  turn_left();
  forward();
  delay(500);
  turn_left();
}

void successive_right_turn() {
  turn_right();
  forward();
  delay(500);
  turn_right();
}

void turnCalibrationR(int start, int ending){
  turnDelayR = start + (ending - start) / 2;
  String input = "";
  turn_right();
  turnDelayR = start + (ending - start) / 2;
  while(Serial.available() == 0) {
  }

  input = Serial.readString();
  if (input == "less") {
    turnCalibrationR(start, start + (ending - start) / 2);
  }
  else if (input == "more") {
    turnCalibrationR(start + (ending - start) / 2, ending);
  } 
  else{
    Serial.println("turnDelayR: ");
    Serial.println(turnDelayR);
  }   
}

//less -> turn less, more -> turn more, anything else to exit
void turnCalibrationL(int start, int ending){
  turnDelayL = start + (ending - start) / 2;
  String input = "";
  turn_left();
  turnDelayL = start + (ending - start) / 2;
  while(Serial.available() == 0) {
  }

  input = Serial.readString();
  if (input == "less") {
    turnCalibrationL(start, start + (ending - start) / 2);
  }
  else if (input == "more") {
    turnCalibrationL(start + (ending - start) / 2, ending);
  } 
  else{
    Serial.println("turnDelayL: ");
    Serial.print(turnDelayL);
    turnCalibrationR(0, 450);    
  }
}

//enter l if path tilts left, r if path tilts right, some variant of y, yes, ok otherwise
void leftMotorCalibration(int start, int ending) {
  
  speedL = start + (ending - start) / 2;
  String input = "";
  forward();
  delay(3000);
  halt();
  while(Serial.available() == 0) {
  }
  //input = direction of movement 
  input = Serial.readString();
  if (input == "l") {
     leftMotorCalibration(start + (ending - start) / 2, ending);
  }
  else if (input == "r") {
    leftMotorCalibration(start,start + (ending - start) / 2);
  }
  else {
    Serial.print("speedL: ");
    Serial.print(speedL);
    turnCalibrationL(0, 450);
  }
}
void setup() {
  Serial.begin(9600);
  Serial.println("Calibration");
  Serial.println("Calibrating speed of left motor...");
  leftMotorCalibration(100, 255);
}

void loop() {
  
}
