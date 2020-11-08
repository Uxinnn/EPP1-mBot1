#include "MeMCore.h"

/* Code to calibrate colour sensor and motor of mBot 
   Colour calibration code is adapted from Week 10 Studio 1*/

// Motor variables
MeDCMotor motorL(M2);  // Left motor
MeDCMotor motorR(M1);  // Right motor
int speedL = 245;  // Speed of left motor, to calibrate
int speedR = 255;  // Speed of right motor, to calibrate
int turnDelayL = 280;  // In milliseconds, to calibrate
int turnDelayR = 280;  // In milliseconds, to calibrate

// Colour sensor variables
MeLightSensor lightSensor(PORT_6);
MeRGBLed led(PORT_7);
#define DELAY 75 // Milliseconds

float whiteArray[] = {0,0,0};
float blackArray[] = {0,0,0};
float colourArray[] = {0,0,0};
float greyDiff[] = {0,0,0};

// true for colour, false for motor
bool calibration_mode = true;
long start;  // To keep time

void setup() {
  Serial.begin(9600);

  start = millis();
  Serial.println("Press button to skip colour calibration.");
  while (millis()-start < 5000) {
    if (analogRead(7) == 0) {
    calibration_mode = !calibration_mode;
    break;
    }
  }

  // Colour calibration. Will be skipped if button is pressed.
  if (calibration_mode) {
    Serial.println("Colour calibration");
    setBalance();  // Colour sensor calibration
    
    // Print results
    Serial.println("COLOUR RESULTS: ");
    Serial.println("GreyDiff:");
    for (int i=0; i<3; i++) {
      Serial.print(greyDiff[i]);
      Serial.print("; ");
    }
    Serial.println();
    Serial.println("blackArray:");
    for (int i=0; i<3; i++) {
      Serial.print(blackArray[i]);
      Serial.print("; ");
    }
    Serial.println();
    Serial.println("WhiteArray:");
    for (int i=0; i<3; i++) {
      Serial.print(whiteArray[i]);
      Serial.println("; ");
    } 
  } else {
    Serial.println("MOTOR MODE");
    motor_test();
  }
}

void loop() {
  start = millis();
  while (millis()-start < 3000) {
    if (analogRead(7) == 0) {
    calibration_mode = !calibration_mode;
    break;
    }
  }
  if (calibration_mode) {
    Serial.println("COLOUR MODE");
    get_colour();
  } else {
    Serial.println("MOTOR MODE");
    motor_test();
  }
  delay(1000);
}


// ================================ //
// Functions for colour calibration //
// ================================ //

// Setting values for white and black
void setBalance(){
  led.setColor(0, 0, 0); led.show();  // Ensure led is off

  // Set white balance
  Serial.println("Put White Sample For Calibration...");
  delay(5000);  // Delay for 5 seconds for getting sample ready
  for (int i = 0; i < 3; i++) {
    led.setColor((i==0) ? 255:0, (i==1) ? 255:0,(i==2) ? 255:0); led.show();
    delay(DELAY);
    whiteArray[i] = getAvgReading(5);
    led.setColor(0, 0, 0); led.show();
    delay(DELAY);
  }

  // Set black balance
  Serial.println("Put Black Sample For Calibration...");
  delay(5000);
  for (int i = 0; i < 3; i++) {
    led.setColor((i==0) ? 255:0, (i==1) ? 255:0,(i==2) ? 255:0); led.show();
    delay(DELAY);
    blackArray[i] = getAvgReading(5);
    led.setColor(0, 0, 0); led.show();
    delay(DELAY);
  }

  // The differnce between the maximum and the minimum gives the range
  for(int i = 0; i < 3; i++){ 
     greyDiff[i] = whiteArray[i] - blackArray[i];
  }
}

// Returns the average value of input number of readings
int getAvgReading(int times){      
  // Find the average reading for the requested number of times of scanning LDR
  int total = 0;
  // Fake the reading as many times as requested and add them up
  for(int i = 0; i < times; i++){
     total += lightSensor.read();
     delay(10);
  }
  // Calculate the average and return it
  return total/times;
}

// Returns the code of detected colour based on calculated values
char get_colour(){
  Serial.println("COLOUR CHALLENGE:");
  for(int c = 0;c<3;c++){
    led.setColor((c==0) ? 255:0, (c==1) ? 255:0,(c==2) ? 255:0); led.show();
    delay(DELAY);
    colourArray[c] = getAvgReading(5);  // Get intensity of light
    colourArray[c] = (colourArray[c] - blackArray[c])/(greyDiff[c])*255;
    led.setColor(0, 0, 0); led.show();
    delay(DELAY);
  }

  // Logging
  for (int i=0;i<3;i++) {
    Serial.print(int(colourArray[i]));
    Serial.print("; ");
  }
  Serial.println();

  // Thresholding to deduce colour shown
  // -------------------------------
  // | 'R' = RED    | 'P' = PURPLE |
  // | 'G' = GREEN  | 'K' = BLACK  |
  // | 'B' = BLUE   | 'E' = ERROR  |
  // | 'Y' = YELLOW | 'N' = NIL    |
  // -------------------------------
  if (colourArray[0] > 200 && colourArray[1] > 200 && colourArray[2] > 200) {
    Serial.println('N');
    return 'N';
  } else if (colourArray[0] > 220) {
    Serial.println('Y');
    return 'Y';
  } else if (colourArray[0] < 20 && colourArray[1] < 20) {
    Serial.println('K');
    return 'K';
  } else if (colourArray[0] < 90) {
    Serial.println('G');
    return 'G';
  } else if (colourArray[1] < 90) {
    Serial.println('R');
    return 'R';
  } else if (colourArray[1] > 180) {
    Serial.println('B');
    return 'B';
  } else if (colourArray[2] > 110) {
    Serial.println('P');
    return 'P';
  } else {
    Serial.println('E');
    return 'E';
  }
}


// =============================== //
// Functions for motor calibration //
// =============================== //

// Send various move commands to mBot to test precision of moves
void motor_test() {
  Serial.println("turn_right");
  turn_right();
  delay(1000);
  Serial.println("turn_left");
  turn_left();
  delay(1000);
  Serial.println("turn_around");
  turn_around();
  delay(1000);
  Serial.println("forward");
  forward();
  motorL.stop();
  motorR.stop();
  delay(1000);
  Serial.println("successive_left_turn");
  successive_left_turn();
  delay(1000);
  Serial.println("successive_right_turn");
  successive_right_turn();
  delay(1000);
}

void  turn_right(){
  motorL.run(-speedL);
  motorR.run(-speedR);
  delay(turnDelayR);
  motorL.stop();
  motorR.stop();
}

void turn_left() {
  motorL.run(speedL);
  motorR.run(speedR);
  delay(turnDelayL);
  motorL.stop();
  motorR.stop();
}

void turn_around() {
  turn_right();
  motorL.stop();
  motorR.stop();
  delay(10);
  turn_right();
}

void forward() {
  motorL.run(-speedL);
  motorR.run(speedR);
  delay(500);
}

void successive_left_turn() {
  turn_left();
  forward();
  turn_left();
}

void successive_right_turn() {
  turn_right();
  forward();
  turn_right();
}
