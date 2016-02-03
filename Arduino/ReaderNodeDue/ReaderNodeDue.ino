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

#define tonePin	   10
#define toneHz	   440
#define toneLength 500

#define highPin    51
#define toneSwitchPin 53

#define BUFSIZE    10  // Size of receive buffer (in bytes) (10-byte unique ID + null character)
#define TABLESIZE  101  // Size of receive buffer (in bytes) (10-byte unique ID + null character)
#define TIMEOUT 2000

#define RFID_START  0x0A  // RFID Reader Start and Stop bytes
#define RFID_STOP   0x0D


String current = "";
//String previous = "";

String rfidData;
bool alarmOn;

unsigned long timeoutTable[TABLESIZE];
char idTable[TABLESIZE][BUFSIZE];

void setup()  // Set up code called once on start-up
{
  pinMode(tonePin, OUTPUT);
  noTone(tonePin);
  // define pin modes
  pinMode(enablePin, OUTPUT);
  pinMode(rxPin, INPUT);
  
  pinMode(highPin, OUTPUT);
  pinMode(toneSwitchPin, INPUT);
  digitalWrite(highPin, HIGH);
  
  alarmOn = false;
  
   for(int i = 0; i<TABLESIZE; i++){
	  timeoutTable[i] = 0;
  }
  
    rfidData.reserve(BUFSIZE);  // Buffer for incoming data

  digitalWrite(enablePin, HIGH);  // disable RFID Reader
  
  // setup Arduino Serial Monitor
  Serial.begin(9600);
  while (!Serial);   // wait until ready
  Serial.println("READER READY");
  
  // set the baud rate for the SoftwareSerial port
  Serial1.begin(2400);

  //Serial.flush();   // wait for all bytes to be transmitted to the Serial Monitor
  
  
  digitalWrite(enablePin, LOW);   // enable the RFID Reader
  
  // Wait for a response from the RFID Reader
  // See Arduino readBytesUntil() as an alternative solution to read data from the reader
  

  
 
}

void loop()  // Main code, to run repeatedly
{
  
  
	
    while (Serial1.available() > 0) // If there are any bytes available to read, then the RFID Reader has probably seen a valid tag
    {
      char nextByte = Serial1.read();  // Get the byte and store it in our buffer
      
      if (nextByte == RFID_START)    // If we receive the start byte from the RFID Reader, then get ready to receive the tag's unique ID
      {
        rfidData = "";
      }
      else if (nextByte == RFID_STOP)  // If we receive the stop byte from the RFID Reader, then the tag's entire unique ID has been sent
      {
		tagComplete();
      }
	  else if(nextByte == 0);
      else{
		  rfidData += nextByte;
	  }
	  
	  if(rfidData.length() > BUFSIZE)
		  rfidData = "CORRUPT";
    }
	
	while(Serial.available() > 1){
		if(Serial.read() == 'A' && Serial.read() == 'R')
			alarmOn = true;
	}
  
	unsigned long nowMS = millis();
	for(int i = 0; i<TABLESIZE; i++){
	  if(timeoutTable[i] != 0){
		  if(nowMS - timeoutTable[i] > TIMEOUT){
			  //Serial.print("Timeout - ");
			  Serial.println(cToInoString(idTable[i]) + ">");
			  timeoutTable[i] = 0;
		  }
	  }
		  
		  
		else{
			
		}
	}
  
 
  
}

void tagComplete(){
	//previous = current;
    current = String(rfidData);
		
	if(timeoutTable[idHash(current)] == 0){
		if(digitalRead(toneSwitchPin) == HIGH)
			tone(tonePin, toneHz, toneLength);
		Serial.println(current + "<");
		//Serial.println(idHash(current));
	}
	//Serial.flush();                 // Wait for all bytes to be transmitted to the Serial Monitor
	
	timeoutTable[idHash(current)] = millis();
	current.toCharArray(idTable[idHash(current)], BUFSIZE+1);
}
//FIX HASHING FUNCTION
int idHash(String id){
	int result = 1;
	for(int i = 0; i < BUFSIZE; i++){
		result += id.charAt(i);
	}
	result = result % TABLESIZE;
}

String cToInoString(char* cstring){
	int i = 0;
	String inoString = "";
	while(cstring[i] != 0){
		inoString += cstring[i];
		i++;
	}
		
	return inoString;
}


/* 
    When the RFID Reader is active and a valid RFID tag is placed with range of the reader,
    the tag's unique ID will be transmitted as a 12-byte printable ASCII string to the host
    (start byte + ID + stop byte)
    
    For example, for a tag with a valid ID of 0F0184F07A, the following bytes would be sent:
    0x0A, 0x30, 0x46, 0x30, 0x31, 0x38, 0x34, 0x46, 0x30, 0x37, 0x41, 0x0D
    
    We'll receive the ID and convert it to a null-terminated string with no start or stop byte. 
  */   







