//systemController.ino
//Jan 29 2018
//DIYDisco

#include "SoftEasyTransfer.h"
#include "SoftwareSerial.h"
#include <Servo.h>
#include <string.h>

SoftwareSerial xbee(2,3); //configure pins 2 and 3 as a serialPort
int led = 13; //debug LED
int cameraServoPin = 9; //pin 9 is servo PWM
String dataIn; // buffer variable for serial intake
SoftEasyTransfer ET;


//define mydata structure
struct SEND_DATA_STRUCTURE{
  char device; //device to control
  char unit; //unique identifier
  char control[3];//control string
};

SEND_DATA_STRUCTURE mydata;//declare mydata as defined mydata structure

//create servo object
Servo camera;   

void setup (){
  xbee.begin(57600);//open some ports
  Serial.begin(57600);
  pinMode(led, OUTPUT); //set pin13 as a status LED
  camera.attach(cameraServoPin);  // attach the servo on pin 9 to the servo object
  ET.begin(details(mydata), &xbee);//start the EasyTransfer library 
}

void loop() {
  //outer loop constantly looks for serial data and executes a command when there is a carriage return
  while (Serial.available() > 0){
    char recieved = Serial.read();
    dataIn += recieved; //concatonate incoming chars

    // Process message when new line or carriage return character is recieved
    if( (recieved == '\n') ||(recieved == '\r')){
      Serial.print("Local Command: ");
      Serial.println(dataIn);
      parseAndPackage(dataIn);//parse the buffer into mydata components
      dataIn="";// Clear recieved buffer
      ET.sendData();//broadcast the mydata
      processmydata();//look for commands to be executed locally
      delay(100);

    }//end of line feed/ carriage return loop
  }//end while
}//end of main loop


/**
 *parseAndPackage(String rawData)
 *Takes serial String and parses into packet components
 *Packet Structure [DEVICE 1byte | UNIT 1 byte | CONTROL CODE 4bytes ]
 *returns: Nothing
 */
void parseAndPackage(String rawData){
  mydata.device = rawData[0]; //first char is the identifier
  mydata.unit = rawData[1]; //second char is the unit
  for (int i=0; i< (sizeof(mydata.control));i++){    //the rest is the control message ; chop the return carriage
    mydata.control[i] = '\0';
    mydata.control[i] = rawData[i+2];
  }
  //just some output for the user
  Serial.print("Device Code: ");
  Serial.println(mydata.device);
  Serial.print("Unit Number: ");
  Serial.println(mydata.unit);
  Serial.print("Control Code: ");
  Serial.println(mydata.control); 
  Serial.print("Packet Size (bytes) : ");
  Serial.println(sizeof(mydata));
  Serial.print("control Size (bytes) : ");
  Serial.println((sizeof(mydata.control)));
}


/**
 *flash(int nFlashes, int flashLength, int spaceLength)
 *flashes the LED on pin 13
 *returns: Nothing
 */
void flash(int nFlashes, int flashLength, int spaceLength) {
  for (int i = 0; i < nFlashes; i++){ 
    digitalWrite(led, HIGH);
    delay(flashLength);
    digitalWrite(led, LOW);
    delay(spaceLength);
  }
}

/**
 *processmydata(String control)
 *determines if local commands are to be executed.
 *returns: Nothing
 */
void processmydata(){
  //since camera control is local, we need to check the package here
  if (mydata.device == 'C'){ //move the camera  
    int cameraPosition = atoi(mydata.control);
    camera.write(cameraPosition);
    //flash(cameraPosition, 1000,1000);
  }

  //flash debugger
  if (mydata.device == 'F'){
    //illuminate debug LED x times, for y millisec with z interval 
    flash((int)mydata.unit-48,atoi(mydata.control),atoi(mydata.control));
  }
}
