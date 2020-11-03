#include <MeMCore.h>

MeLightSensor lightSensor(PORT_6);
MeRGBLed led(PORT_7, 2);

// Define time delay before the next RGB colour turns ON to allow LDR to stabilize
#define DELAY 200 //in milliseconds 

// Define time delay before taking another LDR reading
#define LDRWait 10 //in milliseconds 

//placeholders for colour detected
int red = 0;
int green = 0;
int blue = 0;

int red1 = 255;
int green1 = 255;
int blue1 = 255;

//floats to hold colour arrays
float colourArray[] = {0,0,0};
float whiteArray[] = {0,0,0};
float blackArray[] = {0,0,0};
float greyDiff[] = {0,0,0};

char colourStr[3][5] = {"R = ", "G = ", "B = "};

void setup(){
  //begin serial communication
  Serial.begin(9600);
  
  setBalance();  //calibration
  led.setColor(50, 50, 50); //Check Indicator -- ON after Calibration
  led.show();
  colorSensor();
}

void loop(){
}

void colorSensor(){
  for(int i = 0; i <= 50; i++){
//turn on one colour at a time and LDR reads 5 times
    for(int c = 0;c<=2;c++){    
      Serial.print(colourStr[c]); //print the colour string R=, B=...
      if(c == 0){ //turn ON the LED, red, green or blue, one colour at a time.
        led.setColor(red1, 0, 0);
      } else if(c == 1){
        led.setColor(0, green1, 0);
      } else {
        led.setColor(0, 0, blue1);
      }
      led.show();
      delay(DELAY);
//get the average of 5 consecutive readings for the current colour and return an average 
      colourArray[c] = getAvgReading(5);
//the average reading returned minus the lowest value divided by the maximum possible range, multiplied by 255 will give a value between 0-255, representing the value for the current reflectivity (i.e. the colour LDR is exposed to)
      colourArray[c] = (colourArray[c] - blackArray[c])/(greyDiff[c])*255;
      led.setColor(0, 0, 0);  //turn off the current LED colour
      led.show();
      delay(DELAY);
      Serial.println(int(colourArray[c])); //show the value for the current colour LED, which corresponds to either the R, G or B of the RGB code
   } 
  }
}

void setBalance(){
//set white balance
  Serial.println("Put White Sample For Calibration ...");
  delay(5000);           //delay for five seconds for getting sample ready
  led.setColor(0, 0, 0); //Check Indicator OFF during Calibration
  led.show();
//scan the white sample.
//go through one colour at a time, set the maximum reading for each colour -- red, green and blue to the white array

  led.setColor(red1, 0, 0);
  led.show();
  delay(DELAY);
  whiteArray[0] = getAvgReading(5);
  led.setColor(0, 0, 0);
  led.show();
  delay(DELAY);

  led.setColor(0, green1, 0);
  led.show();
  delay(DELAY);
  whiteArray[1] = getAvgReading(5);
  led.setColor(0, 0, 0);
  led.show();
  delay(DELAY);

  led.setColor(0, 0, blue1);
  led.show();
  delay(DELAY);
  whiteArray[2] = getAvgReading(5);
  led.setColor(0, 0, 0);
  led.show();
  delay(DELAY);

//done scanning white, time for the black sample.
//set black balance
  Serial.println("Put Black Sample For Calibration ...");
  delay(5000);     //delay for five seconds for getting sample ready 
//go through one colour at a time, set the minimum reading for red, green and blue to the black array

  led.setColor(red1, 0, 0);
  led.show();
  delay(DELAY);
  blackArray[0] = getAvgReading(5);
  led.setColor(0, 0, 0);
  led.show();
  delay(DELAY);

  led.setColor(0, green1, 0);
  led.show();
  delay(DELAY);
  blackArray[1] = getAvgReading(5);
  led.setColor(0, 0, 0);
  led.show();
  delay(DELAY);

  led.setColor(0, 0, blue1);
  led.show();
  delay(DELAY);
  blackArray[2] = getAvgReading(5);
  led.setColor(0, 0, 0);
  led.show();
  delay(DELAY);

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
     delay(LDRWait);
  }
//calculate the average and return it
  return total/times;
}
