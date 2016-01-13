/*
  Web client

 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen

 */

#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>

//#include <Time.h>  

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(159,203,112,6);  // numeric IP for Google (no DNS)
char server[] = "www.cyclesentry.xyz";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
SoftwareSerial xbeeSerial(2, 3); // RX, TX

//TODO string completeion, look at serial event online.
boolean stringComplete = false;  // whether the string is complete
String incoming = "";

void setup() {
  
  // Open serial communications and wait for port to open:

  Serial.println("Connecting to USB...");
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

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

  

  
  //"GET /api/echo/"

  //time_t t = now(); 
  //for(int i = 0; i<1; i++){
    //makeRequest(newStr, true);

  //}

  //t = now() - t;

  Serial.println("READY");

  
  
}

void makeRequest(String req){
  Serial.print("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.print("connected. ");
    // Make a HTTP request:
    client.println(req);
    Serial.println("Sent request: " + req);
   
    client.println("Host: www.cyclesentry.xyz");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }

  client.stop();

  
}

void makeRequest(String req, bool printResponse){
  Serial.print("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.print("connected. ");
    // Make a HTTP request:
    client.println(req);
    Serial.println("Sent request: " + req);
   
    
    //client.println("Host: www.cyclesentry.xyz");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }

  if(printResponse)
    getResponse();
  else
    client.stop();
}

String getResponse(){
  while(client.connected()){
    if(client.available()){
      char c = client.read();
      Serial.print(c);
    }
    
  }
  Serial.println("disconnecting.");
  client.stop();

  return "";
}

void loop() {
  //char const* tag = "999";

  /*if (xbeeSerial.available()) {
    String foo = xbeeSerial.readString();
    Serial.println("Received string: " + foo);
    //makeRequest("GET /api/echo/" + foo, true);
  }*/

  while(xbeeSerial.available() > 0){
    char next = xbeeSerial.read();
    if(next == '\n'){
      stringComplete = true;
      break;
    }

    else
      incoming += next;
    
    
  }

  if (stringComplete) {
    //Serial.println(incoming + "-END");
	makeRequest("GET /api/updateTag/" + incoming.substring(0,10)+ "/1 HTTP/1.1", true);
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

 /*// if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }*/

