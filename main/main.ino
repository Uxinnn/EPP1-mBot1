#include "MeMCore.h"

// Motor variables
MeDCMotor motorL(M2);  // left motor
MeDCMotor motorR(M1);  // right motor
int speedL = 245;  // speed of left motor
int speedR = 255;  // speed of right motor
int turnDelayL = 280;  // in milliseconds, trial and error
int turnDelayR = 280;

// Black line detector variables
MeLineFollower lineFinder(PORT_2);  // Black line sensor

// Colour sensor variables
MeLightSensor lightSensor(PORT_6);  // Light sensor
MeRGBLed led(PORT_7);  // Led
float greyDiff[] = {169,158,172};  // Edit after calibration
float blackArray[] = {236, 190, 212};  // Edit after calibration
float colourArray[] = {0,0,0};
char c = 'N';  // Variable to store colour code of detected colour

// Ultrasonic variables
MeUltrasonicSensor ultr(PORT_3);  // Ultrasound sensor
int ULTR_DIST_LIMIT = 6;  // Below this value will cause mbot to go backwards to prevent collision

// IR variables
#define ir_left_pin A0  // left
#define ir_right_pin A1  // right
int IR_DIST_LIMIT_LEFT = 615;  // Below this value will cause nudge
int IR_DIST_LIMIT_RIGHT = 615;  // Below this value will cause nudge

// Tune variables
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_G4  392
#define NOTE_C5  523
MeBuzzer buzzer;
int melody[] = {NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_C5, NOTE_B4};
int noteDurations[] = {8,8,4,4,4,4};

void setup() {
  Serial.begin(9600);  // Serial prints to be removed after code is finalised!
  motorL.run(-speedL);
  motorR.run(speedR);
}

void loop() {
  // If mbot detects a black line, stop and perform waypoint challenge
  if (check_line(lineFinder)) {
    c = 'N';
    motorL.stop();
    motorR.stop();
    delay(200);
    c = get_colour();
    move(c);
    
    motorL.run(-speedL);
    motorR.run(speedR);
  }
  
  // If mbot gets too close to a wall in front, code will run
  if (ultrasonic_sensor(ultr)) {
    motorL.stop();
    motorR.stop();
    delay(100);
    motorL.run(speedL/2);
    motorR.run(-speedR/2);
    delay(400);
    motorL.stop();
    motorR.stop();
  }

  // If mbot gets too close to either side walls, code will run
  if (infrared_sensor() == 1) {
    // nudge right if mbot is too far left
    Serial.println("NUDGE RIGHT");
    
    motorL.run(-255);
    motorR.run(100);
    delay(100);
    motorL.run(-speedL);
    motorR.run(speedR);
  }
  if (infrared_sensor() == 2) {
    // nudge left if mbot is too far right
    Serial.println("NUDGE LEFT");

    motorR.run(255);
    motorL.run(-100);
    delay(100);
    motorL.run(-speedL);
    motorR.run(speedR);
  }
  
  motorL.run(-speedL);
  motorR.run(speedR);
}


//--------------CONTROLS--------------//
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

void move(char c) {
  // React accordingly to colour detected
  switch(c) {
    case 'R':
    turn_left();
    break;
  case 'G':
    turn_right();
    break;
  case 'Y':
    turn_around();
    break;
  case 'P':
    successive_left_turn();
    break;
  case 'B':
    successive_right_turn();
    break;
  case 'K':
    // Play end tune and exit
    play();
    exit(0);
    break;
  case 'E':
    // Error
    c = get_colour();
    move(c);
    break;
  case 'N':
    // Nothing above mbot, false trigger
    break;
  default:
    c = get_colour();
    move(c);
  }
}


//-----------LINE DETECTOR------------//
// Returns true if black line is detected, else return false
bool check_line(MeLineFollower lineFinder) {
  int sensorState = lineFinder.readSensors();
  return sensorState == S1_IN_S2_IN || sensorState == S1_OUT_S2_IN || sensorState == S1_IN_S2_OUT;
}

//-----------COLOUR DETECTOR----------//
// Returns colour code of colour detected by light sensor
// -------------------------------
// | 'R' = RED    | 'P' = PURPLE |
// | 'G' = GREEN  | 'K' = BLACK  |
// | 'B' = BLUE   | 'E' = ERROR  |
// | 'Y' = YELLOW | 'N' = NIL    |
// -------------------------------

char get_colour(){
  Serial.println("COLOUR CHALLENGE:");
  for(int i = 0; i < 3; i++){
    led.setColor((i==0) ? 255:0, (i==1) ? 255:0,(i==2) ? 255:0); led.show();
    delay(150);
    colourArray[i] = getAvgReading(5);
    colourArray[i] = (colourArray[i] - blackArray[i])/(greyDiff[i])*255;
    led.setColor(0, 0, 0); led.show();
    delay(150);
  }

  // Logging
  for (int i = 0; i < 3; i++) {
    Serial.print(int(colourArray[i]));
    Serial.print("; ");
  }
  Serial.println();

  // Threshold values are determined empirically
  if (colourArray[0] > 200 && colourArray[1] > 200 && colourArray[2] > 200) {
    Serial.println('N');
    return 'N';
  } else if (colourArray[0] > 230) {
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
  } else if (colourArray[1] > 160) {
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

int getAvgReading(int times){      
  // Find the average reading for the requested number of times of scanning LDR
  int total = 0;
  // Take the reading as many times as requested and add them up
  for(int i = 0; i < times; i++){
     total += lightSensor.read();
     delay(10);
  }
  //calculate the average and return it
  return total/times;
}

//---------ULTRASONIC DETECTOR--------//
// Returns true if mbot gets too close to a front wall, else false
bool ultrasonic_sensor(MeUltrasonicSensor ultr) {
  int distance = ultr.distanceCm();

  // Logging
  Serial.print("Ultrasonic distance: ");
  Serial.println(distance);

  return distance <= ULTR_DIST_LIMIT;
}

//------------IR DETECTOR-------------//
// Returns 1 if mbot is too close to a left wall, 2 if too close to a right wall, else 0
int infrared_sensor() {
  int left_dist = analogRead(ir_left_pin);
  int right_dist = analogRead(ir_right_pin);

  // Logging
  Serial.print(left_dist);
  Serial.print(", ");
  Serial.println(right_dist);

  if (left_dist <= IR_DIST_LIMIT_LEFT) {
    return 1;  // 1 for nudge right
  }
  if (right_dist <= IR_DIST_LIMIT_RIGHT) {
    return 2;  // 2 for nudge left
  }
  return 0;
}

//----------------TUNE----------------//
// Plays completion tune
void play() {
  for (int thisNote = 0; thisNote < 29; thisNote++) {
    int noteDuration = 1000/noteDurations[thisNote];
    buzzer.tone(8, melody[thisNote],noteDuration);
  
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    buzzer.noTone(8);
  }
}
