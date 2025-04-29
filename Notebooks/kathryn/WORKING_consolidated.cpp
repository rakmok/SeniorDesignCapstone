#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NAU7802.h>
#include <Wire.h>
#include <AccelStepper.h> 

// WiFi Connection
const char* ssid = "Omkar's phone";
const char* password = "12345678";

// const char* ssid = "Kathryn's iPhone";
// const char* password = "kitkat25";

const char* serverIP = "172.20.10.2";
// const char* serverIP = "172.20.10.5";  

const int port = 8080;

// RFID
#define SS_PIN 5
#define RST_PIN 22
MFRC522 rfid(SS_PIN, RST_PIN);
byte lastUid[4] = {0};

//for the presence sensors
// presence sensor b is the bottom, a is the top
#define presence_sensor_a 34
#define presence_sensor_b 35
int sensorStatea = 0, lastStatea=0; 
int sensorStateb = 0, lastStateb=0; 

//motor code check if these are correct motor 1 is the food store
#define dirPin1  33
#define stepPin1  32
int stepsPerRevolution = 200; // 360 degrees = 200 steps

#define dirPin2 26
#define stepPin2 25

// Define AccelStepper motor interface type
#define motorInterfaceType 1 // 1 = Driver (step+dir)

// Create AccelStepper instance for Motor 2
AccelStepper motor2(motorInterfaceType, stepPin2, dirPin2);


//Load Cells & helper function
TwoWire I2C_A = TwoWire(0);      // hardware controller 0

Adafruit_NAU7802 adcA;

//Button
const int buttonPin = 27;
bool lastButtonState = HIGH; // Assume unpressed initially
bool currentButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // 50ms debounce time

// Global flag: becomes true for one loop when a click is detected
bool buttonPressed = false;


bool configADC(Adafruit_NAU7802 &adc) {
  adc.setLDO(NAU7802_3V3);
  adc.setGain(NAU7802_GAIN_1);
  adc.setRate(NAU7802_RATE_10SPS);

  for (uint8_t i = 0; i < 10; i++) {
    while (!adc.available()) delay(1);
    adc.read();
  }

  while (!adc.calibrate(NAU7802_CALMOD_INTERNAL)) {
    Serial.println(F("Internal cal failed, retrying…"));
    delay(200);
  }

  while (!adc.calibrate(NAU7802_CALMOD_OFFSET)) {
    Serial.println(F("System cal failed, retrying…"));
    delay(200);
  }
  return true;
}


// helper function for sending and receiving data
String send_data(String payload) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + String(serverIP) + ":" + String(port) + "/receive";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    Serial.println("payload is " + payload);
    Serial.println("Sending POST now");
    int code = http.POST(payload);

    if (code > 0) {
      String response = http.getString();
      Serial.println("POST Response: " + response);
      return response;
    } else {
      Serial.println("POST Failed. Code: " + String(code));
      return "";
    }
    http.end();
  }    
  return "";
}


// Setup
void setup() {
  delay(2000);
  Serial.begin(115200);
  delay(100);
  SPI.begin();
  rfid.PCD_Init();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  //for the presense sensor
  pinMode(presence_sensor_a, INPUT);  
  pinMode(presence_sensor_b, INPUT);  

  //for the motors
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);

  motor2.setMaxSpeed(1000); 
  motor2.setAcceleration(500); 

  //for the load cells START
  delay(50);
  Serial.println(F("\nDual-NAU7802 demo"));

  I2C_A.begin(17, 16, 400000);  // First ADC: SDA 17, SCL 16

  if (!adcA.begin(&I2C_A)) {
    Serial.println(F("ADC-A (bus 0) not found at 0x2A"));
    while (1) delay(1000);
  }
  Serial.println(F("ADC bowl found ✔"));

  configADC(adcA);
  Serial.println(F("Calibration complete — streaming…"));
  //load cells END

  //button
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  
  //for the food store
  sensorStatea = digitalRead(presence_sensor_a);
  if (sensorStatea != lastStatea) {
    Serial.println(sensorStatea ? "sensor a is Unbroken" : "sensor a is Broken");
    lastStatea = sensorStatea; 

    String payload = "{\"sensor\": \"a\", \"id\": " + String(1582741251) + ", \"value\": " + String(sensorStatea) + "}";
    String response = send_data(payload);
  }

  sensorStateb = digitalRead(presence_sensor_b);
  if (sensorStateb != lastStateb) {
    Serial.println(sensorStateb ? "sensor b is Unbroken" : "sensor b is Broken");
    lastStateb = sensorStateb;  // update last known state
    
    String payload = "{\"sensor\": \"b\", \"id\": " + String(1582741251) + ", \"value\": " + String(sensorStateb) + "}";
    String response = send_data(payload);
  }

  while (!adcA.available()) delay(1);
  int32_t rawA = adcA.read();

  // //TESTING DISPENSE 1 FOOD
  // digitalWrite(dirPin1, HIGH);

  // // Move Motor 1: 180 degrees = 100 steps manually
  // for (int i = 0; i < stepsPerRevolution / 2; i++) {
  //   digitalWrite(stepPin1, HIGH);
  //   delayMicroseconds(2000);  // Speed control
  //   digitalWrite(stepPin1, LOW);
  //   delayMicroseconds(2000);
  // }
  // delay(3000);


  //--------------------------- DO NOT EDIT ABOVE THIS LINE -----------------------------
  //ADD BUTTON LOGIC HERE v
  //TODO: MUST HAVE A BOOLEAN VARIABLE RESULTING FROM THIS WHERE TRUE = BUTTON PRESSED
  buttonPressed = false; // Reset at the start of each loop

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != currentButtonState) {
      currentButtonState = reading;

      if (currentButtonState == LOW || currentButtonState == HIGH) {
        buttonPressed = true; // Set true for this loop if clicked
      }
    }
  }

  lastButtonState = reading;
  //STOP BUTTON LOGIC HERE^

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial() || !buttonPressed) {
    delay(200); 
    return;
  } else {

  

  // if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial() && buttonPressed) {
//   if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // we only get here if the button is pressed and the rfid is found
    String uidString = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10) uidString += "0";
        uidString += String(rfid.uid.uidByte[i], HEX);
    }

    uidString.toUpperCase();
    Serial.println("Detected Tag: " + uidString);
    
    //SEND HOW FULL THE BOWL IS TO THE SERVER -> DONE
    String payload = "{\"tag\": \"" + uidString + "\", \"sensor\": \"load cell\", \"value\": " + String(rawA) + "}";
    String response = send_data(payload);
    //USE RESPONSE TO FIND HOW MUCH TO DISPENSE 
    //  -> i.e. parse response for amount
    int amount_idx = response.indexOf("amount");
    int amount = 0;
    if (amount_idx != -1) {
      int startQuote = response.indexOf("\"", amount_idx + 7);  // after 'amount":"
      int endQuote = response.indexOf("\"", startQuote + 1);
      amount = response.substring(startQuote + 1, endQuote).toInt();   // check if indexing is correct, if it is add (.toInt()) to end of .substring
      
      Serial.print("Amount is: ");
      Serial.println(amount);
    }

    if (amount <= 0) {
      delay(200); 
      return;
    }
    //TODO: ENTER MOTOR CODE TO OPEN THE FOOD BOWL LID HERE v
    int steps90deg = stepsPerRevolution / 4; // 90 degrees = 50 steps

    // Move forward 90 degrees
    motor2.move(steps90deg);
    motor2.runToPosition();
    //END MOTOR CODE TO OPEN THE FOOD BOWL LID HERE ^

    delay(2000);   //wait 2 seconds for bowl to finish opening

    //TODO: ENTER MOTOR CODE TO DISPENSE FOR AMOUNT NEEDED -> DONE
    digitalWrite(dirPin1, HIGH);

    // Move Motor 1: 180 degrees = 100 steps manually
    for (int j = 0; j < amount; j++)
      for (int i = 0; i < stepsPerRevolution / 2; i++) {
        digitalWrite(stepPin1, HIGH);
        delayMicroseconds(2000);  // Speed control
        digitalWrite(stepPin1, LOW);
        delayMicroseconds(2000);
      }

    delay(60000);    //wait 15 minutes for pet to eat

    // delay(900000);    //wait 15 minutes for pet to eat


    //TODO: ENTER MOTOR CODE TO CLOSE FOOD BOWL LID HERE v
    motor2.move(-steps90deg);
    motor2.runToPosition();
    //END MOTOR CODE TO CLOSE FOOD BOWL LID HERE ^


    //TODO: WEIGH THE FOOD BOWL, SEND WEIGHT TO SERVER -> DONE
    //DEBUG: if there are issues, check if the problem is with access/define rawA
    while (!adcA.available()) delay(1);
    rawA = adcA.read();

    delay(50);
    payload = "{\"tag\": \"" + uidString + "\", \"sensor\": \"load cell\", \"value\": " + String(rawA) + ", \"done_eating\": \"true\"}";
    // payload = "{\"id\": " + String(1582741251) + ", \"sensor\": \"load cell\", \"value\": " + String(rawA) + "}";
    response = send_data(payload);

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

  // TODO: parse the response for "POST Response: {"status": "received", "dispense_food": "True", "amount": "3"}"
  // if deispense food is true: 
  //    open the lid of the food bowl (stepper motor code for lid)
  //    delay(2000) (to make sure that it finishes opening)
  //    for the amount that must be dispensed:
  //        rotate the food wheel (stepper motor code for the food)
  //    delay(600000)   (wait 10 minutes and close the food lid)
  //    close the lid of the food bowl (stepper motor code for the lid)
  //    check the weight of the food bowl
  //    send to the server (empty/not)

  // String payload = "{\"id\": \"" + uidString + "\", \"sensor\": \"load cell\", \"value\": " + String(rawA) + ", \"done_eating\": true}";
  // String response = send_data(payload);

  }  
  delay(1000); 
}
