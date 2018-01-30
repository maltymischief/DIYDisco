/**********************************************************************************************
 *GenericNode
 *Chris M. Ward
 *1/29/2018
 *
 *
 *DESCRIPTION:
 *This sketch controls a Generic Node.
 *Recieved commands are re-packetized and  executed.
 *SoftEasyTransfer converts packet structure to a binary string, and handles CRC.
 *Possible commands: flash indicator, sound audible alert,  dispense a treat,
 *                   dispense a meal, cancel command
 *
 *
 *CHANGE LOG:
 *r1: oct2012
 *r2: jan2014 -changed from single char command to more robust 
 * packet-structure commands
 *          
 **********************************************************************************************/
#include <Servo.h> 
#include <string.h>
#include <EasyTransfer.h>


char UNIT_NUMBER = '1';

EasyTransfer ET; //create object
Servo myservo; // create servo object to control a servo 
int led = 13;

struct RECEIVE_DATA_STRUCTURE{ //define mydata structure
  char device; //device to control
  char unit; //unique identifier
  char control[5]; //control string
};

//give a name to the group of data
RECEIVE_DATA_STRUCTURE mydata;

void setup(){  

  // initialize serial communication://///////////////////////////////
  Serial.begin(57600);
  ET.begin(details(mydata), &Serial); 
  pinMode(led, OUTPUT); 
  pinMode(speaker, OUTPUT);

  myservo.attach(10);
  myservo.write(90);
  delay(300);
}

void loop() {
  //check and see if a data mydata has come in. 
  if(ET.receiveData()){

    Serial.print("Device Code: ");
    Serial.println(mydata.device);
    Serial.print("Unit Number: ");
    Serial.println(mydata.unit);
    Serial.print("Control Code: ");
    String temp = String(mydata.control);
    Serial.println(temp.substring(0,sizeof(mydata.control))); 
    //Serial.print("Packet Size (bytes) : ");
    //Serial.println(sizeof(mydata));

    processmydata(temp);
  }
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


void processmydata(String control){

  //illuminate debug LED x times, for y millisec with z interval 
  if (mydata.device == 'F'){
    flash((int)mydata.unit-48,atoi(mydata.control),atoi(mydata.control));
  }
 
  //dispense kibble
  if (mydata.device == 'S'){ //activate servo
    
    //feeder 1 or 2 or both
    if((mydata.unit == UNIT_NUMBER)||(mydata.unit == 'B')){

      //quantity to dispense
      if(control.substring(0,sizeof(mydata.control)) == "SMALL"){
        myservo.write(99);
        Serial.print("Unit ");
        Serial.print(UNIT_NUMBER); 
        Serial.println(" Moving a little. ");
        delay(1600);
        myservo.write(90);
      }

      if(control.substring(0,sizeof(mydata.control)-2) == "LRG"){ //"LRG" is only three letters in a five element array
        myservo.write(0);
        Serial.print("Unit ");
        Serial.print(UNIT_NUMBER); 
        Serial.println(" Moving more. ");
        delay(2200);
        myservo.write(90);
      }
    }
  }           

}     












