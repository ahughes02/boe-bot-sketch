/*
ARUDINO MEGA POWERED ROBOT
----------------------------------------------
THIS PROGRAM GATHERS ALLOWS A USER TO CONTROL
THE ROBOT WITH SERIAL COMMANDS
REQURIES AN ARDUINO MEGA
----------------------------------------------

Programmed by: Austin Hughes
Modified last: 2014-04-18
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

#define BUFSIZE  16  // Size of buffer (in bytes) for incoming data from the LRF (this should be adjusted to be larger than the expected response)

// servos
Servo servoRight;	// Declare right servo
Servo servoLeft;	// Declare left servo
Servo lrfServo;     // servo for the laser range finder

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
boolean sensor = true; //stores if the prox sensor is active
int prox = 0; // stores proximity
String stopString = "#1500,1500$";


// References
// Sample Line of data from serial
//  L     R
// #1500,1500$
// 1300ms full speed CW, 1500ms stops, 1700ms full speed CCW.

//SoftwareSerial btSerial(2, 3); // RX, TX SOFTWARE SERIAL NOT WORKING ON THE ARDUINO MEGA

void setup() 
{
  // initialize serial ports
  Serial.begin(9600);
  Serial1.begin(9600);
  //btSerial.begin(9600);
  
  tone(4, 3000, 1000);                       // Play tone for 1 second
  delay(1000);                               // Delay to finish tone
  
  // start servos
  servoRight.attach(12);                     // Attach right signal to pin 12 
  servoRight.writeMicroseconds(1500);        // 1.5 ms stop
  
  servoLeft.attach(13);                     // Attach left signal to pin 13 
  servoLeft.writeMicroseconds(1500);        // 1.5 ms stop
  
  delay(1000);
  
  /*
    When the LRF powers on, it launches an auto-baud routine to determine the
    host's baud rate. it will stay in this state until a "U" ($55) character
    is sent by the host. 
  */
  
  // uncomment the serial lines when debugging
  //Serial.print("Waiting for the LRF...");
  delay(2000);                        // Delay to let LRF module start up
  Serial1.print('U');               // Send character
  while (Serial1.read() != ':');    // When the LRF has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  Serial1.flush();                  // Flush the receive buffer
 // Serial.println("Ready!");
 // Serial.flush();                     // Wait for all bytes to be transmitted to the Serial Monitor
  
  delay(1000);
  
  lrfServo.attach(11);
  lrfServo.writeMicroseconds(1400); // move laser to center
  
  Serial.println("1"); // debug
  
  //Wire.begin();
  
  // Proximity sensor setup
  //uint8_t rev = read8(VCNL4000_PRODUCTID);
  
  /* if ((rev & 0xF0) != 0x10) 
   {
     Serial.println("Proximity sensor not found");
     
     tone(4, 3000, 500);             
     tone(4, 3000, 1000);                
     tone(4, 3000, 500); */
     
     sensor = false;
   //}
   
    Serial.println("2"); // debug
   
   /*if(sensor)
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
   }*/
   Serial.println("3"); // debug
   tone(4, 3000, 500);                       // Play tone for 1 second
   delay(500);                               // Delay to finish tone
}

void loop() 
{
    
    // --------------------------
    // serial message validation
    // --------------------------
    while(exitLoop != 1)
    {
      //prox = readProximity(); proximitiy sensor not working yet
	  
      //if(prox > 3000 && leftMSint < 1500 && rightMSint > 1500)
      //{
      //  Serial.print("Prox = ");
      //  Serial.println(prox);
      //  stopString.toCharArray(buffer, 20);
      //  exitLoop = 1;
      //}
      
      count = 0; // reset count
          
      incByte = Serial.peek(); // get the next byte
          
      test = char(incByte); // store it as a character 
	  
      if(test == 's')
      {
        Serial.println("Sweep command received");
        Serial.read();
	sweep();
      }
 
      if(test == '#' && exitLoop != 1) // if the next byte is a '#' then its the start of a new string
      {
        Serial.readBytesUntil('$', buffer, 20); // read until the stop character
        
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
          Serial.print('c'); // confirmation message
        }
      }
      else
      {
        Serial.read(); // read to clear buffer
      }
    }
	
      
      // Raw buffer print out uncomment for debug
     // Serial.println("");
     // Serial.print("Raw buffer: ");
     // Serial.println(buffer);
     // Serial.println("");
       
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
    
      // uncomment lines below for debug
      
      //Send leftMS to Computer
      //Serial.print("Left: ");
      //Serial.println(leftMS);
          
      //Send rightMS to computer
      //Serial.print("Right: ");
      //Serial.println(rightMS);
          
      leftMSint = atoi(leftMS);
      rightMSint = atoi(rightMS);
	  
      //if(prox > 3000 && leftMSint < 1500 && rightMSint > 1500)
      //{
      //  Serial.print("Prox = ");
      // Serial.println(prox);
      //  leftMSint = 1500;
		//rightMSint = 1500;
      //  exitLoop = 1;
      //}
 
      servoRight.writeMicroseconds(rightMSint);
      servoLeft.writeMicroseconds(leftMSint);
      
      exitLoop = 0;
}

// moves the laser sensor and takes readings three times
void sweep()
{
  readProx();
  
  delay(1000);
  
  lrfServo.writeMicroseconds(2000);
  
  readProx();
  
  delay(1000);
  
  lrfServo.writeMicroseconds(900);
  
  delay(1000);
  
  readProx();
  
  delay(1000);
  
  lrfServo.writeMicroseconds(1400); // center?
}

// Read poximity from the laser range finder
void readProx()
{
    /* 
    When a single range (R) command is sent, the LRF returns the distance to the target
    object in ASCII in millimeters. For example:
     
    D = 0123 mm
  */   
  Serial1.print('R');         // Send command
  
  // Get response back from LRF
  // See Arduino readBytesUntil() as an alternative solution to read data from the LRF
  char lrfData[BUFSIZE];  // Buffer for incoming data
  char offset = 0;        // Offset into buffer
  lrfData[0] = 0;         // Clear the buffer    
  while(1)
  {
    if (Serial1.available() > 0) // If there are any bytes available to read, then the LRF must have responded
    {
      lrfData[offset] = Serial1.read();  // Get the byte and store it in our buffer
      if (lrfData[offset] == ':')          // If a ":" character is received, all data has been sent and the LRF is ready to accept the next command
      {
        lrfData[offset] = 0; // Null terminate the string of bytes we just received
        break;               // Break out of the loop
      }
          
      offset++;  // Increment offset into array
      if (offset >= BUFSIZE) offset = 0; // If the incoming data string is longer than our buffer, wrap around to avoid going out-of-bounds
    }
  }
  Serial.println(lrfData);    // The lrfData string should now contain the data returned by the LRF, so display it on the Serial Monitor
  //btSerial.println(lrfData); // switched to hardware serial instead of software so leave this line commented out unless software serial is used.
  Serial.flush();             // Wait for all bytes to be transmitted to the Serial Monitor
}

/* ***************************************

// Infrared Proximity Sensor Methods

   ***************************************/
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

// Read 1 byte from the VCNL4000 at 'address'
uint8_t read8(uint8_t address)
{
  uint8_t data;

  Wire.beginTransmission(VCNL4000_ADDRESS);
#if ARDUINO >= 100
  Wire.write(address);
#else
  Wire.send(address);
#endif
  Wire.endTransmission();

  delayMicroseconds(170);  // delay required

  Wire.requestFrom(VCNL4000_ADDRESS, 1);
  while(!Wire.available());

#if ARDUINO >= 100
  return Wire.read();
#else
  return Wire.receive();
#endif
}


// Read 2 byte from the VCNL4000 at 'address'
uint16_t read16(uint8_t address)
{
  uint16_t data;

  Wire.beginTransmission(VCNL4000_ADDRESS);
#if ARDUINO >= 100
  Wire.write(address);
#else
  Wire.send(address);
#endif
  Wire.endTransmission();

  Wire.requestFrom(VCNL4000_ADDRESS, 2);
  while(!Wire.available());
#if ARDUINO >= 100
  data = Wire.read();
  data <<= 8;
  while(!Wire.available());
  data |= Wire.read();
#else
  data = Wire.receive();
  data <<= 8;
  while(!Wire.available());
  data |= Wire.receive();
#endif
  
  return data;
}

// write 1 byte
void write8(uint8_t address, uint8_t data)
{
  Wire.beginTransmission(VCNL4000_ADDRESS);
#if ARDUINO >= 100
  Wire.write(address);
  Wire.write(data);  
#else
  Wire.send(address);
  Wire.send(data);  
#endif
  Wire.endTransmission();
}
