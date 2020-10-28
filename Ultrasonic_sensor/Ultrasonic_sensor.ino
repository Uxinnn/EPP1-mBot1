#include <MeMCore.h>

MeUltrasonicSensor ultr(PORT_3);

int distance_limit = 10
void ultrasonic_sensor() {
  int distance = ultr.distanceCm();
  Serial.println(distance);
  if (distance <= distance_limit) {
    motor_left.stop();
    motor_right.stop();
  }
}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
