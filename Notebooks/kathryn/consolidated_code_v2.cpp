'''
This is the consolidated code for the ECE445 Team 30 Group Project.

To Do:
    - find button click
    - open lid
    - spin food dispenser wheel
    - after time, close lid
    - weigh food bowl
    - send to server the weight
'''

#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NAU7802.h>

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

//Load Cells
Adafruit_NAU7802 nau;

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
        } else {
          Serial.println("POST Failed. Code: " + String(code));
          String response = "";
        }
        http.end();
      }    
      return response;
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
  Wire.begin(17, 16);
  Serial.print("NAU7802");
  if (! nau.begin()) {
    Serial.print("Failed to find NAU7802");
    while (1) delay(10);  // Don't proceed.
  }
  Serial.print("Found NAU7802");

  nau.setLDO(NAU7802_3V0);
  Serial.print("LDO voltage set to ");
  switch (nau.getLDO()) {
    case NAU7802_4V5:  Serial.println("4.5V"); break;
    case NAU7802_4V2:  Serial.println("4.2V"); break;
    case NAU7802_3V9:  Serial.println("3.9V"); break;
    case NAU7802_3V6:  Serial.println("3.6V"); break;
    case NAU7802_3V3:  Serial.println("3.3V"); break;
    case NAU7802_3V0:  Serial.println("3.0V"); break;
    case NAU7802_2V7:  Serial.println("2.7V"); break;
    case NAU7802_2V4:  Serial.println("2.4V"); break;
    case NAU7802_EXTERNAL:  Serial.println("External"); break;
  }

  nau.setGain(NAU7802_GAIN_128);
  Serial.print("Gain set to ");
  switch (nau.getGain()) {
    case NAU7802_GAIN_1:  Serial.println("1x"); break;
    case NAU7802_GAIN_2:  Serial.println("2x"); break;
    case NAU7802_GAIN_4:  Serial.println("4x"); break;
    case NAU7802_GAIN_8:  Serial.println("8x"); break;
    case NAU7802_GAIN_16:  Serial.println("16x"); break;
    case NAU7802_GAIN_32:  Serial.println("32x"); break;
    case NAU7802_GAIN_64:  Serial.println("64x"); break;
    case NAU7802_GAIN_128:  Serial.println("128x"); break;
  }

  nau.setRate(NAU7802_RATE_10SPS);
  Serial.print("Conversion rate set to ");
  switch (nau.getRate()) {
    case NAU7802_RATE_10SPS:  Serial.println("10 SPS"); break;
    case NAU7802_RATE_20SPS:  Serial.println("20 SPS"); break;
    case NAU7802_RATE_40SPS:  Serial.println("40 SPS"); break;
    case NAU7802_RATE_80SPS:  Serial.println("80 SPS"); break;
    case NAU7802_RATE_320SPS:  Serial.println("320 SPS"); break;
  }

  // Take 10 readings to flush out readings
  for (uint8_t i=0; i<10; i++) {
    while (! nau.available()) delay(1);
    nau.read();
  }

  while (! nau.calibrate(NAU7802_CALMOD_INTERNAL)) {
    Serial.println("Failed to calibrate internal offset, retrying!");
    delay(1000);
  }
  Serial.println("Calibrated internal offset");

  while (! nau.calibrate(NAU7802_CALMOD_OFFSET)) {
    Serial.println("Failed to calibrate system offset, retrying!");
    delay(1000);
  }
  Serial.println("Calibrated system offset");
  //for the load cells END
}

void loop() {
  //for the food store
  sensorStatea = digitalRead(presence_sensor_a);
  if (sensorStatea != lastStatea) {
    Serial.println(sensorStatea ? "sensor a is Unbroken" : "sensor a is Broken");
    lastStatea = sensorStatea; 

    String payload = "{\"sensor\": \"a\", \"id\": " + String(1582741251) + ", \"value\": " + String(sensorStatea) + "}";
    String response = send_data(payload);
    Serial.println("POST Response: " + response);
  }

  sensorStateb = digitalRead(presence_sensor_b);
  if (sensorStateb != lastStateb) {
    Serial.println(sensorStateb ? "sensor b is Unbroken" : "sensor b is Broken");
    lastStateb = sensorStateb;  // update last known state
    
    String payload = "{\"sensor\": \"b\", \"id\": " + String(1582741251) + ", \"value\": " + String(sensorStateb) + "}";
    String response = send_data(payload);
    Serial.println("POST Response: " + response);
  }

  //load cells
  while (! nau.available()) {
    delay(1);
  }
  int32_t val = nau.read();
  Serial.print("Read "); Serial.println(val);

  //rfid not found 
  // TODO: AND BUTTON NOT PRESSED
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(200); 
    return;
  }

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
  Serial.println("POST Response: " + response);

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
  delay(1000); 
}
