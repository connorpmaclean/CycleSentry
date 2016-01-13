/*
  
   
  RFID Code by:
  Parallax RFID Reader: Basic Demonstration      
  Author: Joe Grand [www.grandideastudio.com]             
  Contact: support@parallax.com 
  1.0 (April 30, 2014): Initial release
  
*/

#include <DueTone.h>

#define enablePin  9   // Connects to the RFID's ENABLE pin
#define rxPin      19  // Serial input (connects to the RFID's SOUT pin)
#define txPin      20  // Serial output (unused)

#define BUFSIZE    11  // Size of receive buffer (in bytes) (10-byte unique ID + null character)

#define RFID_START  0x0A  // RFID Reader Start and Stop bytes
#define RFID_STOP   0x0D


String current = "";
String previous = "";

void setup()  // Set up code called once on start-up
{
  pinMode(10, OUTPUT);
  noTone(10);
  // define pin modes
  pinMode(enablePin, OUTPUT);
  pinMode(rxPin, INPUT);
  
  pinMode(51, OUTPUT);
  pinMode(53, INPUT);
  digitalWrite(51, HIGH);
  

  digitalWrite(enablePin, HIGH);  // disable RFID Reader
  
  // setup Arduino Serial Monitor
  Serial.begin(9600);
  while (!Serial);   // wait until ready
  Serial.println("READER READY");
  
  // set the baud rate for the SoftwareSerial port
  Serial1.begin(2400);

  Serial.flush();   // wait for all bytes to be transmitted to the Serial Monitor
}

void loop()  // Main code, to run repeatedly
{
  digitalWrite(enablePin, LOW);   // enable the RFID Reader
  
  // Wait for a response from the RFID Reader
  // See Arduino readBytesUntil() as an alternative solution to read data from the reader
  char rfidData[BUFSIZE];  // Buffer for incoming data
  char offset = 0;         // Offset into buffer
  rfidData[0] = 0;         // Clear the buffer    
  
  while(1)
  {
    if (Serial1.available() > 0) // If there are any bytes available to read, then the RFID Reader has probably seen a valid tag
    {
      rfidData[offset] = Serial1.read();  // Get the byte and store it in our buffer
      
      if (rfidData[offset] == RFID_START)    // If we receive the start byte from the RFID Reader, then get ready to receive the tag's unique ID
      {
        offset = -1;     // Clear offset (will be incremented back to 0 at the end of the loop)
      }
      else if (rfidData[offset] == RFID_STOP)  // If we receive the stop byte from the RFID Reader, then the tag's entire unique ID has been sent
      {
		previous = current;
        rfidData[offset] = 0; // Null terminate the string of bytes we just received
        current = String(rfidData);
        break;                // Break out of the loop
      }
          
      offset++;  // Increment offset into array
      if (offset >= BUFSIZE) offset = 0; // If the incoming data string is longer than our buffer, wrap around to avoid going out-of-bounds
    }
  }
	
  if(!previous.equals(current)){
	  if(digitalRead(53) == HIGH)
		tone(10, 440, 500);
	  Serial.println(current);
  }
	
  
  //Serial.println(previous + ", " + current);       // The rfidData string should now contain the tag's unique ID with a null termination, so display it on the Serial Monitor
  Serial.flush();                 // Wait for all bytes to be transmitted to the Serial Monitor
}


/* 
    When the RFID Reader is active and a valid RFID tag is placed with range of the reader,
    the tag's unique ID will be transmitted as a 12-byte printable ASCII string to the host
    (start byte + ID + stop byte)
    
    For example, for a tag with a valid ID of 0F0184F07A, the following bytes would be sent:
    0x0A, 0x30, 0x46, 0x30, 0x31, 0x38, 0x34, 0x46, 0x30, 0x37, 0x41, 0x0D
    
    We'll receive the ID and convert it to a null-terminated string with no start or stop byte. 
  */   







