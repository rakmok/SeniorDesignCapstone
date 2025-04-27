'''
this is the working consolidated code. Working:
    - rfid
    - wifi
    - presence sensors
    - load cell

TODO:
    - need to modify app side to push notifications AFTER LID CLOSES of remaining food amount (as a percentage of bowl capacity)
    - need to modify esp side to rotate by the amount provided
    - need to add a button and button code
    - need to add motor code

'''

#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NAU7802.h>
#include <Wire.h>

// WiFi Connection
const char* ssid = "Omkar's phone";
const char* password = "12345678";

const char* serverIP = "172.20.10.2";  
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

//Load Cells & helper function
TwoWire I2C_A = TwoWire(0);      // hardware controller 0

Adafruit_NAU7802 adcA;

bool configADC(Adafruit_NAU7802 &adc) {
  adc.setLDO(NAU7802_3V0);
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

  //for the load cells START
  delay(50);
  Serial.println(F("\nDual-NAU7802 demo"));

  I2C_A.begin(17, 16, 400000);  // First ADC: SDA 17, SCL 16

  if (!adcA.begin(&I2C_A)) {
    Serial.println(F("❌  ADC-A (bus 0) not found at 0x2A"));
    while (1) delay(1000);
  }
  Serial.println(F("ADC bowl found ✔"));

  configADC(adcA);
  Serial.println(F("Calibration complete — streaming…"));
  //load cells END
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

  //load cells
  while (!adcA.available()) delay(1);
  int32_t rawA = adcA.read();

  delay(50);
  String payload = "{\"sensor\": \"load cell\", \"value\": " + String(rawA) + "}";
  String response = send_data(payload);


  //rfid not found 
  // TODO: AND BUTTON NOT PRESSED
  // if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
  //   delay(200); 
  //   return;
  // }

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // we only get here if the button is pressed and the rfid is found
    String uidString = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10) uidString += "0";
        uidString += String(rfid.uid.uidByte[i], HEX);
    }

    uidString.toUpperCase();
    Serial.println("Detected Tag: " + uidString);
    
    String payload = "{\"tag\": \"" + uidString + "\"}";
    String response = send_data(payload);

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

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
  delay(1000); 
}
