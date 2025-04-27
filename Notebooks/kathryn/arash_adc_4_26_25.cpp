/*********************************************************************
  Dual-NAU7802 demo for ESP32-WROOM-32E
  ─ Two independent I²C buses
      ADC-A  SDA = GPIO17, SCL = GPIO16  (Wire instance 0)
      ADC-B  SDA = GPIO26, SCL = GPIO27  (Wire instance 1)

  ─ Same configuration applied to both chips:
      LDO 3.0 V, Gain 128 ×, 10 SPS, full offset calibration
*********************************************************************/

#include <Wire.h>
#include <Adafruit_NAU7802.h>

/* ----------  bus objects & ADC objects  ---------- */
TwoWire I2C_A = TwoWire(0);      // hardware controller 0
TwoWire I2C_B = TwoWire(1);      // hardware controller 1

Adafruit_NAU7802 adcA;
Adafruit_NAU7802 adcB;

/* ----------  helper to configure ONE ADC  ---------- */
bool configADC(Adafruit_NAU7802 &adc) {

  /* 1.  LDO voltage */
  adc.setLDO(NAU7802_3V0);

  /* 2.  Gain */
  adc.setGain(NAU7802_GAIN_128);

  /* 3.  Sample-per-second rate */
  adc.setRate(NAU7802_RATE_10SPS);

  /* 4.  Throw away a few startup readings */
  for (uint8_t i = 0; i < 10; i++) {
    while (!adc.available()) delay(1);
    adc.read();
  }

  /* 5.  Internal offset calibration */
  while (!adc.calibrate(NAU7802_CALMOD_INTERNAL)) {
    Serial.println(F("Internal cal failed, retrying…"));
    delay(200);
  }

  /* 6.  System offset calibration (with scale unloaded) */
  while (!adc.calibrate(NAU7802_CALMOD_OFFSET)) {
    Serial.println(F("System cal failed, retrying…"));
    delay(200);
  }

  return true;
}

void setup() {

  Serial.begin(115200);
  delay(50);
  Serial.println(F("\nDual-NAU7802 demo"));

  /* ----------  bring up the two I²C buses  ---------- */
  I2C_A.begin(17, 16, 400000);  // First ADC: SDA 17, SCL 16
  I2C_B.begin(25, 33, 400000);  // Second ADC: SDA 25, SCL 33


  /* ----------  probe the ADCs  ---------- */
  if (!adcA.begin(&I2C_A)) {
    Serial.println(F("❌  ADC-A (bus 0) not found at 0x2A"));
    while (1) delay(1000);
  }
  if (!adcB.begin(&I2C_B)) {
    Serial.println(F("❌  ADC-B (bus 1) not found at 0x2A"));
    while (1) delay(1000);
  }
  Serial.println(F("Both ADCs found ✔"));

  /* ----------  identical configuration on each  ---------- */
  configADC(adcA);
  configADC(adcB);
  Serial.println(F("Calibration complete — streaming…"));
}

void loop() {

  /* Non-blocking reads — wait only for the device you’re about to read */
  while (!adcA.available()) delay(1);
  int32_t rawA = adcA.read();

  while (!adcB.available()) delay(1);
  int32_t rawB = adcB.read();

  Serial.printf("%ld\t%ld\n", rawA, rawB);   // tab-separated
  delay(50);                                 // ~20 Hz print rate
}
