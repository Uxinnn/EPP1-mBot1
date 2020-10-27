#include "MeMCore.h"

MeDCMotor motor1(M1);
MeDCMotor motor2(M2);
MeLineFollower lineFinder(PORT_2);

uint8_t motorSpeed = 100;

void setup()
{
  Serial.begin(9600);
  motor1.run(motorSpeed);
  motor2.run(-motorSpeed);
}

void loop()
{
  // Check incoming collision

  // Check for black line
  if (check_line(lineFinder)) {
    motor1.stop();
    motor2.stop();
    // Check colour and move
  }
  delay(200);
}

bool check_line(MeLineFollower lineFinder) {
  int sensorState = lineFinder.readSensors();
  if (sensorState == S1_IN_S2_IN) {
    return true;
  } else {
    return false;
  }
}
