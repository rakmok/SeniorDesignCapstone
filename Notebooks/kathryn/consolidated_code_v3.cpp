"""
working code from last week, switched out the adc code with arash's from 4/26/25
"""

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
TwoWire I2C_B = TwoWire(1);      // hardware controller 1

Adafruit_NAU7802 adcA;
Adafruit_NAU7802 adcB;

bool configADC(Adafruit_NAU7802 &adc) {
  adc.setLDO(NAU7802_3V0);
  adc.setGain(NAU7802_GAIN_128);
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
  I2C_B.begin(25, 33, 400000);  // Second ADC: SDA 25, SCL 33

  if (!adcA.begin(&I2C_A)) {
    Serial.println(F("❌  ADC-A (bus 0) not found at 0x2A"));
    while (1) delay(1000);
  }
  if (!adcB.begin(&I2C_B)) {
    Serial.println(F("❌  ADC-B (bus 1) not found at 0x2A"));
    while (1) delay(1000);
  }
  Serial.println(F("Both ADCs found ✔"));

  configADC(adcA);
  configADC(adcB);
  Serial.println(F("Calibration complete — streaming…"));
  //load cells END
}

void loop() {
  //for the food store
  sensorStatea = digitalRead(presence_sensor_a);

  if (sensorStatea != lastStatea) {
    Serial.println(sensorStatea ? "sensor a is Unbroken" : "sensor a is Broken");

    lastStatea = sensorStatea; 

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = "http://" + String(serverIP) + ":" + String(port) + "/receive";
      http.begin(url);
      http.addHeader("Content-Type", "application/json");

      String payload = "{\"sensor\": \"a\", \"id\": " + String(1582741251) + ", \"value\": " + String(sensorStatea) + "}";
      Serial.println("payload is " + payload);

      Serial.println("Sending POST now");
      int code = http.POST(payload);

      if (code > 0) {
        String response = http.getString();
        Serial.println("POST Response: " + response);
      } else {
        Serial.println("POST Failed. Code: " + String(code));
      }
      http.end();
    }    
  }

  sensorStateb = digitalRead(presence_sensor_b);
  if (sensorStateb != lastStateb) {
    Serial.println(sensorStateb ? "sensor b is Unbroken" : "sensor b is Broken");
    lastStateb = sensorStateb;  // update last known state

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = "http://" + String(serverIP) + ":" + String(port) + "/receive";
      http.begin(url);
      http.addHeader("Content-Type", "application/json");

      String payload = "{\"sensor\": \"b\", \"id\": " + String(1582741251) + ", \"value\": " + String(sensorStateb) + "}";
      Serial.println("payload is " + payload);

      Serial.println("Sending POST now");
      int code = http.POST(payload);

      if (code > 0) {
        String response = http.getString();
        Serial.println("POST Response: " + response);
      } else {
        Serial.println("POST Failed. Code: " + String(code));
      }
      http.end();
    }
  }

  //load cells
  while (!adcA.available()) delay(1);
  int32_t rawA = adcA.read();

  while (!adcB.available()) delay(1);
  int32_t rawB = adcB.read();

  Serial.printf("%ld\t%ld\n", rawA, rawB);   // tab-separated
  delay(50);                                 // ~20 Hz print rate

  //rfid
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(200); 
    return;
  }

  bool isNew = false;
  for (int i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != lastUid[i]) {
      isNew = true;
      break;
    }
  }

  if (isNew) {
    for (int i = 0; i < 4; i++) {
      lastUid[i] = rfid.uid.uidByte[i];
    }

    String uidString = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10) uidString += "0";
      uidString += String(rfid.uid.uidByte[i], HEX);
    }

    uidString.toUpperCase();
    Serial.println("Detected Tag: " + uidString);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = "http://" + String(serverIP) + ":" + String(port) + "/receive";
      http.begin(url);
      http.addHeader("Content-Type", "application/json");
      String payload = "{\"tag\": \"" + uidString + "\", \"rawA\": " + String(rawA) + ", \"rawB\": " + String(rawB) + "}";
      // String payload = "{\"tag\": \"" + uidString + "\"}";
      Serial.println("payload is "+payload);
      
      Serial.println("Sending POST now");
      int code = http.POST(payload);

      if (code > 0) {
        String response = http.getString();
        Serial.println("POST Response: " + response);
      } else {
        Serial.println("POST Failed. Code: " + String(code));
      }
      http.end();
    }
  } else {
    Serial.println("Card already scanned.");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1000); 
}
