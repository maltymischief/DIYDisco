//laserTurret.ino
//Jan 30 2018
//DIYDisco

#include <Servo.h> 

Servo servox;
Servo servoy;
int laser = 13;
int incomingByte;      // a variable to read incoming serial data into

long i;
unsigned long timer;
unsigned long timer2;

void setup() 
{  
  digitalWrite(7, HIGH); //We need to set it HIGH immediately on boot
  pinMode(7,OUTPUT);     //We can declare it an output ONLY AFTER it's HIGH
                  
  // initialize serial communication:
  Serial.begin(9600);
  // initialize the Laser pin as an output:
  pinMode(laser, OUTPUT);

  servox.attach(5);
  servox.write(90);
  servoy.attach(6);
  servoy.write(90);

  //pinMode(5, OUTPUT);
  //analogWrite(5, 128);

  delay(300);
  timer = millis()+900000;
  timer2 = millis()+3600000;
}

void loop() {
  if (Serial.available() > 0) {

    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();
    // if it's a capital H (ASCII 72), turn on the Lase:
    if (incomingByte == 'H') {
      digitalWrite(laser, HIGH);
      if(millis() > timer){
        
        servox.write(90);
        servoy.write(90);
        servox.detach();
        servoy.detach();
        if(millis() > timer2){
          timer2 = millis()+3600000;
          timer = millis()+900000;
          servox.attach(9);
          servoy.attach(10);
          servox.write(90);
          servoy.write(90);
          delay(300);
        }
      }
      if(millis() < timer){

        int CircleXCenter = 90;
        int CircleYCenter = 90;
        int loopDelay = 30;
        checkForKill();
        randomSeed(analogRead(4));
        CircleXCenter = random(80,100);
        randomSeed(analogRead(5));
        CircleYCenter = random(80,100);
        randomSeed(analogRead(6));
        loopDelay = random(15,20);

        for (int Rad = 5; Rad < 20; Rad++)
        {
          for (int i = 0; i < 100; i++)
          {
            float angle = i*2*3.14/100;
            int xPos = CircleXCenter + (cos(angle) * Rad);
            int yPos = CircleYCenter + (sin(angle) * Rad);
            checkForKill();
           

            servox.write(xPos);
            servoy.write(yPos);
            delay(loopDelay);

          }
        }

        randomSeed(analogRead(4));
        CircleXCenter = random(80,100);
        randomSeed(analogRead(5));
        CircleYCenter = random(80,100);
        randomSeed(analogRead(6));
        loopDelay = random(15,20);

        for (int Rad = 40; Rad > 20; Rad--)
        {
          for (int i = 100; i > 0; i--)
          {
            float angle = i*2*3.14/100;
            int xPos = CircleXCenter + (cos(angle) * Rad);
            int yPos = CircleYCenter + (sin(angle) * Rad);

            servox.write(xPos);
            servoy.write(yPos);
            delay(loopDelay);

          }
        }
      }
    }

    // if it's an L (ASCII 76) turn off the LED:
    if (incomingByte == 'L') {
      digitalWrite(laser, LOW);
      servox.write(90);
      servoy.write(90);

    }

  digitalWrite(laser, LOW);
  }
  

}

void checkForKill() {
   incomingByte = Serial.read();
  if (incomingByte == 'L') {
    //Serial.println("STOP");
    digitalWrite(7, LOW); //Pulling the RESET pin LOW triggers the reset.
    delay(1000);
  } 
}
