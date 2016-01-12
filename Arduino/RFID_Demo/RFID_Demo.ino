/*
  
  Parallax RFID Reader: Basic Demonstration       
                                                         
  Author: Joe Grand [www.grandideastudio.com]             
  Contact: support@parallax.com                            
  
  Program Description:
  
  This program provides a simple demonstration of the Parallax RFID Card
  Reader (#28140). If a valid RFID tag is placed in front of the reader,
  its unique ID is displayed in the Arduino Serial Monitor. 
  
  Please refer to the product manual for full details of system functionality 
  and capabilities.

  Revisions:
  
  1.0 (April 30, 2014): Initial release
  
*/



#define enablePin  9   // Connects to the RFID's ENABLE pin
#define rxPin      19  // Serial input (connects to the RFID's SOUT pin)
#define txPin      20  // Serial output (unused)

#define BUFSIZE    11  // Size of receive buffer (in bytes) (10-byte unique ID + null character)

#define RFID_START  0x0A  // RFID Reader Start and Stop bytes
#define RFID_STOP   0x0D


void setup()  // Set up code called once on start-up
{
  pinMode(10, OUTPUT);
  noTone(10);
  // define pin modes
  pinMode(enablePin, OUTPUT);
  pinMode(rxPin, INPUT);
  

  digitalWrite(enablePin, HIGH);  // disable RFID Reader
  
  // setup Arduino Serial Monitor
  Serial.begin(9600);
  while (!Serial);   // wait until ready
  Serial.println("\n\nParallax RFID Card Reader");
  
  // set the baud rate for the SoftwareSerial port
  Serial1.begin(2400);

  Serial.flush();   // wait for all bytes to be transmitted to the Serial Monitor
}

void loop()  // Main code, to run repeatedly
{
  /* 
    When the RFID Reader is active and a valid RFID tag is placed with range of the reader,
    the tag's unique ID will be transmitted as a 12-byte printable ASCII string to the host
    (start byte + ID + stop byte)
    
    For example, for a tag with a valid ID of 0F0184F07A, the following bytes would be sent:
    0x0A, 0x30, 0x46, 0x30, 0x31, 0x38, 0x34, 0x46, 0x30, 0x37, 0x41, 0x0D
    
    We'll receive the ID and convert it to a null-terminated string with no start or stop byte. 
  */   
  
  digitalWrite(enablePin, LOW);   // enable the RFID Reader
  //noTone(10);
  
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
        rfidData[offset] = 0; // Null terminate the string of bytes we just received
        
        break;                // Break out of the loop
      }
          
      offset++;  // Increment offset into array
      if (offset >= BUFSIZE) offset = 0; // If the incoming data string is longer than our buffer, wrap around to avoid going out-of-bounds
    }
  }

  tone(10, 440, 500);
  
  Serial.println(rfidData);       // The rfidData string should now contain the tag's unique ID with a null termination, so display it on the Serial Monitor
  Serial.flush();                 // Wait for all bytes to be transmitted to the Serial Monitor
}





/*
Tone generator
v1  use timer, and toggle any digital pin in ISR
   funky duration from arduino version
   TODO use FindMckDivisor?
   timer selected will preclude using associated pins for PWM etc.
    could also do timer/pwm hardware toggle where caller controls duration
*/


// timers TC0 TC1 TC2   channels 0-2 ids 0-2  3-5  6-8     AB 0 1
// use TC1 channel 0
#define TONE_TIMER TC1
#define TONE_CHNL 0
#define TONE_IRQ TC3_IRQn

// TIMER_CLOCK4   84MHz/128 with 16 bit counter give 10 Hz to 656KHz
//  piano 27Hz to 4KHz

static uint8_t pinEnabled[PINS_COUNT];
static uint8_t TCChanEnabled = 0;
static boolean pin_state = false ;
static Tc *chTC = TONE_TIMER;
static uint32_t chNo = TONE_CHNL;

volatile static int32_t toggle_count;
static uint32_t tone_pin;

// frequency (in hertz) and duration (in milliseconds).

void tone(uint32_t ulPin, uint32_t frequency, int32_t duration)
{
    const uint32_t rc = VARIANT_MCK / 256 / frequency;
    tone_pin = ulPin;
    toggle_count = 0;  // strange  wipe out previous duration
    if (duration > 0 ) toggle_count = 2 * frequency * duration / 1000;
    else toggle_count = -1;

    if (!TCChanEnabled) {
      pmc_set_writeprotect(false);
      pmc_enable_periph_clk((uint32_t)TONE_IRQ);
      TC_Configure(chTC, chNo,
        TC_CMR_TCCLKS_TIMER_CLOCK4 |
        TC_CMR_WAVE |         // Waveform mode
        TC_CMR_WAVSEL_UP_RC ); // Counter running up and reset when equals to RC
  
      chTC->TC_CHANNEL[chNo].TC_IER=TC_IER_CPCS;  // RC compare interrupt
      chTC->TC_CHANNEL[chNo].TC_IDR=~TC_IER_CPCS;
      NVIC_EnableIRQ(TONE_IRQ);
                         TCChanEnabled = 1;
    }
    if (!pinEnabled[ulPin]) {
      pinMode(ulPin, OUTPUT);
      pinEnabled[ulPin] = 1;
    }
    TC_Stop(chTC, chNo);
                TC_SetRC(chTC, chNo, rc);    // set frequency
    TC_Start(chTC, chNo);
}

void noTone(uint32_t ulPin)
{
  TC_Stop(chTC, chNo);  // stop timer
  digitalWrite(ulPin,LOW);  // no signal on pin
}

// timer ISR  TC1 ch 0
void TC3_Handler ( void ) {
  TC_GetStatus(TC1, 0);
  if (toggle_count != 0){
    // toggle pin  TODO  better
    digitalWrite(tone_pin,pin_state= !pin_state);
    if (toggle_count > 0) toggle_count--;
  } else {
    noTone(tone_pin);
  }
}



