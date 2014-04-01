/*
ARUDINO POWERED ROBOT
----------------------------------------------
THIS PROGRAM GATHERS ALLOWS A USER TO CONTROL
THE ROBOT WITH SERIAL COMMANDS
----------------------------------------------

Programmed by: Austin Hughes
Modified last: 2014-02-04
*/ 

#include <Servo.h>
#include <SoftwareSerial.h>
#include <Wire.h>

// the i2c address of infrared proximity sensor
#define VCNL4000_ADDRESS 0x13

// commands and constants for infared proximity sensor
#define VCNL4000_COMMAND 0x80
#define VCNL4000_PRODUCTID 0x81
#define VCNL4000_IRLED 0x83
#define VCNL4000_AMBIENTPARAMETER 0x84
#define VCNL4000_AMBIENTDATA 0x85
#define VCNL4000_PROXIMITYDATA 0x87
#define VCNL4000_SIGNALFREQ 0x89
#define VCNL4000_PROXINITYADJUST 0x8A

#define VCNL4000_3M125 0
#define VCNL4000_1M5625 1
#define VCNL4000_781K25 2
#define VCNL4000_390K625 3

#define VCNL4000_MEASUREAMBIENT 0x10
#define VCNL4000_MEASUREPROXIMITY 0x08
#define VCNL4000_AMBIENTREADY 0x40
#define VCNL4000_PROXIMITYREADY 0x20

// servos
Servo servoRight;	// Declare right servo
Servo servoLeft;	// Declare left servo

// variables
byte incByte; 		// stores the next byte in the serial buffer
char test; 		// test character to be compared in the while loop
char buffer[20]; 	// buffer for incoming data 
char leftMS[5]; 	// variable to store leftMS information
char rightMS[5]; 	// variable to store rightMS information
int count; 		// count stores the number of '#' characters in the buffer
int j = 0; 		// iterator to store data in humidity, temp, and pressure variable
int exitLoop = 0; 	// variable is set to 1 when valid buffer is stored
int leftMSint = 1500;   // stores the left microseconds as an integer
int rightMSint = 1500;  // stores the right microseconds as an integer
boolean sensor = true;

// References
// Sample Line of data from serial
//  L     R
// #1500,1500$
// 1300ms full speed CW, 1500ms stops, 1700ms full speed CCW.

SoftwareSerial mySerial(2, 3); // RX, TX

void setup() 
{
  // initialize serial ports
  Serial.begin(9600);
  mySerial.begin(9600);
  
  tone(4, 3000, 1000);                       // Play tone for 1 second
  delay(1000);                               // Delay to finish tone
  
  // start servos
  servoRight.attach(12);                     // Attach right signal to pin 12 
  servoRight.writeMicroseconds(1500);        // 1.5 ms stop
  
  servoLeft.attach(13);                     // Attach left signal to pin 13 
  servoLeft.writeMicroseconds(1500);        // 1.5 ms stop
  
  Wire.begin();
  
   if ((rev & 0xF0) != 0x10) 
   {
     Serial.println("Proximity sensor not found");
     
     tone(4, 3000, 500);             
     tone(4, 3000, 1000);                
     tone(4, 3000, 500);
     
     sensor = false;
   }
   
   if(sensor)
   {
        write8(VCNL4000_IRLED, 20);        // set to 20 * 10mA = 200mA
        Serial.print("IR LED current = ");
        Serial.print(read8(VCNL4000_IRLED) * 10, DEC);
        Serial.println(" mA");
        
        //write8(VCNL4000_SIGNALFREQ, 3);
        Serial.print("Proximity measurement frequency = ");
        uint8_t freq = read8(VCNL4000_SIGNALFREQ);
        if (freq == VCNL4000_3M125) Serial.println("3.125 MHz");
        if (freq == VCNL4000_1M5625) Serial.println("1.5625 MHz");
        if (freq == VCNL4000_781K25) Serial.println("781.25 KHz");
        if (freq == VCNL4000_390K625) Serial.println("390.625 KHz");
        
        write8(VCNL4000_PROXINITYADJUST, 0x81);
        Serial.print("Proximity adjustment register = ");
        Serial.println(read8(VCNL4000_PROXINITYADJUST), HEX);
   }
}

uint16_t readProximity() 
{
  write8(VCNL4000_COMMAND, VCNL4000_MEASUREPROXIMITY);
  while (1) 
  {
    uint8_t result = read8(VCNL4000_COMMAND);
    //Serial.print("Ready = 0x"); Serial.println(result, HEX);
    if (result & VCNL4000_PROXIMITYREADY) 
    {
      return read16(VCNL4000_PROXIMITYDATA);
    }
    delay(1);
  }
}

void loop() 
{
    // --------------------------
    // serial message validation
    // --------------------------
    while(exitLoop != 1)
    {
      count = 0; // reset count
          
      incByte = mySerial.peek(); // get the next byte
          
      test = char(incByte); // store it as a character 

      if(test == '#') // if the next byte is a '#' then its the start of a new string
      {
        mySerial.readBytesUntil('$', buffer, 20); // read until the stop character
        
        // check for the number of '#' chars in the buffer    
        for(int i = 0; i <= 20; i++)
        {
          if(buffer[i] == '#')
          {
            count++;
          }
        }
        
        // if only one '#' is found, the buffer is valid   
        if(count == 1)
        {
          exitLoop = 1;
          mySerial.print('c'); // confirmation message
        }
      }
      else
      {
        mySerial.read(); // read to clear buffer
      }
    }
      
      // Raw buffer print out for debug
      Serial.println("");
      Serial.print("Raw buffer: ");
      Serial.println(buffer);
      Serial.println("");
       
      // --------------------------------
      // loops to save data to variables
      // --------------------------------
          
      // Save leftMS to variable
      j = 0;
      for(int i = 1; i < 5; i++)
      {
        leftMS[j] = buffer[i];
        j++;
      }
          
      // Save rightMS to variable
      j = 0;
      for(int i = 6; i < 10; i++)
      {
        rightMS[j] = buffer[i];
        j++;
      }
          
      // ----------------------------
      // send data over USB
      // ----------------------------
    
      //Send leftMS to Computer
      Serial.print("Left: ");
      Serial.println(leftMS);
          
      //Send rightMS to computer
      Serial.print("Right: ");
      Serial.println(rightMS);
          
      leftMSint = atoi(leftMS);
      rightMSint = atoi(rightMS);
 
      servoRight.writeMicroseconds(rightMSint);
      servoLeft.writeMicroseconds(leftMSint);
      
      exitLoop = 0;
}
