//=====LIBRARIES====
#include <Wire.h>                           //Library for i2c wire connection
#define SLAVE_ADDRESS 0x04                  //Setting Arduino as slave

//====Initializations====
int buttonPin = 2;                          //The pushbutton pin
int potPin = A0;                            //The Potentiometer pin
volatile int buttonState = 1;               //Variable for knowing the Push-Button state
byte dataS[1];                              //Array of data that's being sent to the RPi
int sample = 10;                            //Setting sample size. For any size bigger, it would be best to use the RPi
                                            //Since it has overall faster processing speed t
void setup() {
  Serial.begin(9600);                       //Initializing serial monitor for the data sent to the RPi
  pinMode(buttonPin, INPUT_PULLUP);         //Setting the Push-Button pin as an input
  attachInterrupt(digitalPinToInterrupt(buttonPin), ledPin_ISR, FALLING);    //ISR Interrupt for the Push-Button
  Wire.begin(SLAVE_ADDRESS);                //Setting the Arduino i2c as slave
  Wire.onRequest(sendData);                 //Setting Arduino to send data
}

void loop() {  
  //====CONFIGURATION WHEN SYSTEM IS ON====
  if (buttonState == -1){
    int sum = 0;      //Initializing and reseting the summation
    int potAvg = 0;   //Initializing and reseting the average
    
    //This is for getting an average value of 10 samples
    for (int i = 0; i < sample; i++){
      int potValue = analogRead(potPin);          //Reads Potentiometer value
      //This maps to the value range of 0-100, its 102 to take into account any small error 
      //and to get it preset as a percentage which can be directly taken to the RPi
      int potSend = map(potValue, 0,1023, 1,102); 
      //Summing each new sampled value together
      sum = sum + potSend;
    }
    //Taking the average of the summed sample
    potAvg = sum/sample;
    //Sending the summed sample to the RPi
    dataS[0] = potAvg;
  }
  //====CONFIGURATION WHEN SYSTEM IS OFF====
  else if (buttonState == 1) {
    //When the RPi reads a 0, that means that the system will be turned off, hence another reason for the mapping offset of the potentiometer
    dataS[0] = 0;

    }
    //Can monitor data sent to the RPi to make sure both register properly
    Serial.println(dataS[0]);
  }

//Arduino function for sending data to the RPi
void sendData() {
  Wire.write(dataS,1);
}

//=====BUTTON & BUTTONFLAG INTERRUPT====
//The system is initially off, then turns on as its pushed which triggers the ISR buttonState.
//This cycles back and forth between being off (-1) and on (1)
void ledPin_ISR() {
  if (buttonState == 1) {       //System off
    buttonState = buttonState * -1;
  }
  else if (buttonState == -1) { //System on
    buttonState = buttonState * -1;
  }
}
