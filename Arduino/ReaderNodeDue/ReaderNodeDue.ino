/*
  CycleSentry RFID Reader Node
  By Connor MacLean
  Last Modified: 16-Mar-2016
  
  Code based on:
   
  RFID Code by:
  Parallax RFID Reader: Basic Demonstration      
  Author: Joe Grand [www.grandideastudio.com]             
  Contact: support@parallax.com 
  1.0 (April 30, 2014): Initial release
  
*/

#include <DueTone.h>

#define RFID_ENABLE_PIN 	9   // Connects to the RFID's ENABLE pin
#define RFID_RX_PIN     	19  // Serial input (connects to the RFID's SOUT pin)
#define RFID_TX_PIN     	20  // Serial output (unused)
#define LIGHT_PIN  			13  //Built in Due LED

#define TONE_PIN	    	15  //Buzzer pin
#define TONE_ALARM_HZ	   	440 //Standard tone
#define TONE_ALARM_LENGTH 	500 //Standard tone length

#define TONE_ERROR_HZ		100
#define TONE_ERROR_LENGTH	2000

#define HIGH_PIN    		51
#define SILENCE_PIN 		53

#define BUFSIZE    			10  // Size of receive buffer (in bytes) (10-byte unique ID + null character)
#define TABLESIZE  			101  // Size of receive buffer (in bytes) (10-byte unique ID + null character)
#define TIMEOUT 			2000
#define ALARM_DUR 			400

#define RFID_START  		0x0A  // RFID Reader Start and Stop bytes
#define RFID_STOP   		0x0D

#define rfidSerial 			Serial1
#define xbeeSerial 			Serial
#define RFID_SERIAL_BAUD	2400
#define XBEE_SERIAL_BAUD	9600



String current = "";	//Incoming strings from RFID reader.
String rfidData;
bool alarmOn;			//Boolean indicating the alarm is on
unsigned long lastAlarmTime;

unsigned long timeoutTable[TABLESIZE];	//Table indicating the last time a tag was seen.
char idTable[TABLESIZE][BUFSIZE];		//Corresponding string table to above.

void setup()  // Set up code called once on start-up
{
  pinMode(TONE_PIN, OUTPUT);
  noTone(TONE_PIN);
  pinMode(RFID_ENABLE_PIN, OUTPUT);
  pinMode(RFID_RX_PIN, INPUT);
  
  pinMode(HIGH_PIN, OUTPUT);
  pinMode(SILENCE_PIN, INPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(HIGH_PIN, HIGH);
  
  alarmOn = false;
  
   for(int i = 0; i<TABLESIZE; i++){
	  timeoutTable[i] = 0;
  }
  
  rfidData.reserve(BUFSIZE);  // Buffer for incoming data

  digitalWrite(RFID_ENABLE_PIN, HIGH);  // disable RFID Reader
  
  // setup Arduino Serial Monitor
  xbeeSerial.begin(XBEE_SERIAL_BAUD);
  
  // set the baud rate for the SoftwareSerial port
  rfidSerial.begin(RFID_SERIAL_BAUD);
  
  waitForConnection();
  digitalWrite(RFID_ENABLE_PIN, LOW);   // enable the RFID Reader
 
}

void waitForConnection(){
	xbeeSerial.println("?");
	unsigned long nowMS = millis();
	while(millis() - nowMS < TIMEOUT){
		if(Serial.available() >= 1){
			if(Serial.read() == 'Y'){
				tone(TONE_PIN, TONE_ALARM_HZ, TONE_ERROR_LENGTH);
				return;
			}
			break;
		}
	}
	tone(TONE_PIN, TONE_ERROR_HZ, TONE_ERROR_LENGTH);
}

void loop()  // Main code, to run repeatedly
{
  
  
	//Handles data from the RFID Reader
    while (rfidSerial.available() > 0) // If there are any bytes available to read, then the RFID Reader has probably seen a valid tag
    {
      char nextByte = rfidSerial.read();  // Get the byte and store it in our buffer
      
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
	
	//Handles data from the xbee.
	while(xbeeSerial.available() > 0){
		char inc = Serial.read();
		if(inc == '1'){
			
			alarmOn = true;
			digitalWrite(13, HIGH);
		}
		
		else if(inc == '0'){
			alarmOn = false;
			digitalWrite(13, LOW);
		}
			
	}
	
	//Play Alarm
	if(alarmOn && millis() - lastAlarmTime > ALARM_DUR * 2){
		if(digitalRead(SILENCE_PIN) == HIGH)
			tone(TONE_PIN, 220, ALARM_DUR);	//Play a low tone to indicate its working.
		
		lastAlarmTime = millis();
	}
		
  
	//Update the timeout table.
	unsigned long nowMS = millis();
	for(int i = 0; i<TABLESIZE; i++){
	  if(timeoutTable[i] != 0){
		  //If there is a timeout, send a message with >
		  if(nowMS - timeoutTable[i] > TIMEOUT){
			  xbeeSerial.println(cToInoString(idTable[i]) + ">");
			  timeoutTable[i] = 0;
		  }
	  }
	}
  
 
  
}

//Handles event when a tag from the RFID is completed.
void tagComplete(){
    current = String(rfidData);
		
	if(timeoutTable[idHash(current)] == 0){
		if(digitalRead(SILENCE_PIN) == HIGH)
			tone(TONE_PIN, TONE_ALARM_HZ, TONE_ALARM_LENGTH);	//Tag read tone
		xbeeSerial.println(current + "<");
		//Serial.println(idHash(current));
	}
	
	//Update table.
	timeoutTable[idHash(current)] = millis();
	current.toCharArray(idTable[idHash(current)], BUFSIZE+1);
}

//Simple hashing function
int idHash(String id){
	int result = 1;
	for(int i = 0; i < BUFSIZE; i++){
		result += id.charAt(i);
	}
	result = result % TABLESIZE;
}

//Converts c string to an arduino string.
String cToInoString(char* cstring){
	int i = 0;
	String inoString = "";
	while(cstring[i] != 0){
		inoString += cstring[i];
		i++;
	}
		
	return inoString;
}






