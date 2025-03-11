/* 
  IR Breakbeam sensor demo!
*/

#define SENSORPIN 4

// variables will change:
int sensorState = 0, lastState=0;         // variable for reading the pushbutton status

void setup() {
  // initialize the LED pin as an output:
  pinMode(LED_BUILTIN, OUTPUT);      
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT);     
  //digitalWrite(SENSORPIN, HIGH); // turn on the pullup //comment this out
  
  Serial.begin(9600);
}

void loop(){
  // read the state of the pushbutton value:
  sensorState = digitalRead(SENSORPIN);

  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorState == LOW) {     
    // turn LED on:
    digitalWrite(LED_BUILTIN, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(LED_BUILTIN, LOW); 
  }
  
  if (sensorState) {           //print continuously
    Serial.println("Unbroken");
  } 
  if (!sensorState) {
    Serial.println("Broken");
  }
}
