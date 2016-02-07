#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>

#define ALARM_FREQ 	5000	//How often an alarm check is polled (in ms)
#define TIMEOUT 	500		//Timeout for API requests (in ms)

#define URL "www.cyclesentry.xyz"
#define PORT 80

//#include <Time.h>  

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = URL;    // name address for server (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);	//Not sure if this is correct!

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
SoftwareSerial xbeeSerial(2, 3); // RX, TX


boolean stringComplete = false;  // whether the string is complete
String incoming = "";			 //Holds the incoming string

unsigned long lastAlarmTime;	 //Last time the alarm check was polled.

void setup() {
  
  // Open serial communications and wait for port to open:
	lastAlarmTime = millis();
  
  Serial.begin(9600);
  Serial.println("Connecting to USB...");

  Serial.println("Connecting to XBEE...");
  xbeeSerial.begin(9600);
  
  
  Serial.println("Setting up Ethernet...");
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }

  Serial.println("Initializing Ethernet board...");
  // give the Ethernet shield a second to initialize:
  delay(1000);


  Serial.println("READY!");	//Pronounced in a high-pitched Japanese voice.

}



void loop() {

  /*if (xbeeSerial.available()) {
    String foo = xbeeSerial.readString();
    Serial.println("Received string: " + foo);
    //makeRequest("GET /api/echo/" + foo, true);
  }*/

  //Check for data from xbee
  while(xbeeSerial.available() > 0){
    char next = xbeeSerial.read();
    if(next == '\n'){	//New line indiactes end of message
      stringComplete = true;
      break;
    }

    else
      incoming += next;
    
  }
  
  //Check for alarm update if enough time has passed.
  if(millis()- lastAlarmTime > ALARM_FREQ){
	  char alarmChar = makeRequest("GET /api/random/ HTTP/1.1", true);
	  xbeeSerial.print(alarmChar);
	  Serial.println(alarmChar);
	  lastAlarmTime = millis();
  }

  //Handle a string completion event
  if (stringComplete) {
    //Serial.println(incoming + "-END");
	String locIn = "1";
	String locOut = "-1";
	String loc = "0";
	if(incoming.charAt(10) == '>')
		loc = locOut;
	else
		loc = locIn;
	
	makeRequest("GET /api/updateTag/" + incoming.substring(0,10)+ "/" + loc + " HTTP/1.1", true);
	//makeRequest("GET /api/echo/" + incoming.substring(0,10)+ " HTTP/1.1", true);
	//makeRequest("GET /api/echo/aaa HTTP/1.1", true);
    // clear the string:
    incoming = "";
    stringComplete = false;
  }

  //Get from USB
  /*if(Serial.available()){
    String foo = Serial.readString();
    String newStr = "GET /api/updateTag/" + foo;
    makeRequest(newStr, true);
    makeRequest("GET /api/echo/" + foo, true);
  }*/
}


char makeRequest(String req){
	return makeRequest(req, false);
  
}

char makeRequest(String req, bool needResponse){
  //Serial.print("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, PORT)) {
    //Serial.print("connected. ");
    // Make a HTTP request:
    client.println(req);
    Serial.println("Sent request: " + req);
   
    
    client.println("Host: www.cyclesentry.xyz");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    //Serial.println("connection failed");
  }
	
  char retChar = 0;
  if(needResponse)
    retChar = getResponse();
  client.stop();
  
  return retChar;
  
}

char getResponse(){
	char lastChar = 0;
	
	unsigned long startTime = millis();
  while(client.connected()){
	if(millis() - startTime > TIMEOUT){
		Serial.println("Timeout");
		break;
	}
    if(client.available()){
      lastChar = client.read();
      //Serial.print(c);
    }
    
  }
  //Serial.println("disconnecting.");

  return lastChar;
}


