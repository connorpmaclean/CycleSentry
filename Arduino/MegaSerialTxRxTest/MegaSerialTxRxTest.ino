
void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial1.begin(9600);
  
  pinMode(13, OUTPUT);
  
  
}

bool toggle = true;

void loop() {

  if (Serial1.available()) {
    Serial.write(Serial1.read());
	toggle = !toggle;
	if(toggle)
		digitalWrite(13, HIGH);
	
	else
		digitalWrite(13, LOW);
  }
  if (Serial.available()) {
	byte temp = Serial.read();
    Serial1.write(temp);
	Serial.write(temp);
	delay(10);
  }
  
  
}
