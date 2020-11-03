#include "MeMCore.h"

// Motor variables
MeDCMotor motorL(M2);//left motor
MeDCMotor motorR(M1);//right motor

//Colour sensor variables
MeLightSensor lightSensor(PORT_6);
MeRGBLed led(PORT_7);
#define DELAY 75 //in milliseconds

float whiteArray[] = {0,0,0};
float blackArray[] = {0,0,0};
float colourArray[] = {0,0,0};
float greyDiff[] = {0,0,0};


void setup() {
  Serial.begin(9600);
  
  setBalance();  // colour sensor calibration
  // motor calibration

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
    Serial.print("; ");
  }
  Serial.println();
  Serial.println("MOTOR RESULTS: ");
  // print motor results
}

void loop() {
  get_colour();
  delay(5000);
}


void setBalance(){
  //set white balance
  Serial.println("Put White Sample For Calibration ...");
  delay(5000);           //delay for five seconds for getting sample ready
  led.setColor(0, 0, 0); //Check Indicator OFF during Calibration
  led.show();

  for (int i=0; i<3; i++) {
    led.setColor((i==0) ? 255:0, (i==1) ? 255:0,(i==2) ? 255:0);
    led.show();
    delay(DELAY);
    whiteArray[i] = getAvgReading(5);
    led.setColor(0, 0, 0);
    led.show();
    delay(DELAY);
  }

  Serial.println("Put Black Sample For Calibration ...");
  delay(5000);
  
  for (int i=0; i<3; i++) {
    led.setColor((i==0) ? 255:0, (i==1) ? 255:0,(i==2) ? 255:0);
    led.show();
    delay(DELAY);
    blackArray[i] = getAvgReading(5);
    led.setColor(0, 0, 0);
    led.show();
    delay(DELAY);
  }

  //the differnce between the maximum and the minimum gives the range
  for(int i = 0; i <= 2; i++){ 
     greyDiff[i] = whiteArray[i] - blackArray[i];
  }
}

int getAvgReading(int times){      
  //find the average reading for the requested number of times of scanning LDR
  int reading;
  int total = 0;
  //take the reading as many times as requested and add them up
  for(int i = 0;i < times;i++){
     reading = lightSensor.read();
     total = reading + total;
     delay(10);
  }
  //calculate the average and return it
  return total/times;
}


char get_colour(){
  Serial.println("COLOUR CHALLENGE:");
  for(int c = 0;c<3;c++){
    led.setColor((c==0) ? 255:0, (c==1) ? 255:0,(c==2) ? 255:0);
    led.show();
    delay(DELAY);
    colourArray[c] = getAvgReading(5);
    colourArray[c] = (colourArray[c] - blackArray[c])/(greyDiff[c])*255;
    led.setColor(0, 0, 0);
    led.show();
    delay(DELAY);
  }
  for (int i=0;i<3;i++) {
    Serial.print(int(colourArray[i]));
    Serial.print("; ");
  }
  Serial.println();

  if (colourArray[0] > 200 && colourArray[1] > 200 && colourArray[2] > 200) {
    Serial.println('N');
    return 'N';
  } else if (colourArray[0] > 200) {
    Serial.println('Y');
    return 'Y';
  } else if (colourArray[0] < 20 && colourArray[1] < 20) {
    Serial.println('K');
    return 'K';
  } else if (colourArray[0] < 60) {
    Serial.println('G');
    return 'G';
  } else if (colourArray[1] < 60) {
    Serial.println('R');
    return 'R';
  } else if (colourArray[1] > 130) {
    Serial.println('B');
    return 'B';
  } else if (colourArray[2] > 120) {
    Serial.println('P');
    return 'P';
  } else {
    Serial.println('E');
    return 'E';
  }
}
