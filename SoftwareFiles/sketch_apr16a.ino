#include <Wire.h>
#include <Adafruit_NAU7802.h>

Adafruit_NAU7802 nau;

#define VIN1_THRESHOLD 200000  // bowl press threshold
#define NAU7802_ADDR 0x2A
#define REG_CTRL2 0x02

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Wire.begin(17, 16);   // SDA, SCL
  pinMode(21, INPUT);   // DRDY

  Serial.println("Initializing NAU7802...");
  if (!nau.begin())
  {
    Serial.println("Failed to find NAU7802");
    while (1) delay(10);
  }

  nau.setLDO(NAU7802_3V3);
  nau.setGain(NAU7802_GAIN_128);
  nau.setRate(NAU7802_RATE_10SPS);

  for (int i = 0; i < 10; i++)
  {
    while (!nau.available()) delay(1);
    nau.read();
  }

  while (!nau.calibrate(NAU7802_CALMOD_INTERNAL)) delay(1000);
  while (!nau.calibrate(NAU7802_CALMOD_OFFSET)) delay(1000);

  setChannel(false);  // Start with VIN1 (bowl)
}

void loop()
{
  // Always monitor VIN1 (bowl)
  setChannel(false);
  while (!nau.available()) delay(1);
  int32_t vin1_val = nau.read();

  Serial.print("VIN1 (bowl): ");
  Serial.println(vin1_val);

  if (vin1_val > VIN1_THRESHOLD)
  {
    Serial.println("Bowl pressed! Monitoring button for 10 seconds...");

    // Switch to VIN2 (button)
    setChannel(true);
    delay(50); // allow channel to settle

    unsigned long startTime = millis();
    int32_t last_button_val = 0;

    while (millis() - startTime < 10000)
    {
      while (!nau.available()) delay(1);
      int32_t vin2_val = nau.read();
      last_button_val = vin2_val;
      Serial.print("VIN2 (button): ");
      Serial.println(vin2_val);
      delay(200); // pace reads
    }

    Serial.print("Last VIN2 reading after 10 sec: ");
    Serial.println(last_button_val);

    // Switch back to VIN1
    setChannel(false);
    delay(500); // debounce or reset time
  }

  delay(100); // loop pacing
}

void setChannel(bool vin2)
{
  Wire.beginTransmission(NAU7802_ADDR);
  Wire.write(REG_CTRL2);
  Wire.endTransmission(false);
  Wire.requestFrom(NAU7802_ADDR, 1);
  if (Wire.available())
  {
    uint8_t ctrl2 = Wire.read();
    if (vin2)
      ctrl2 |= 0b10000000;
    else
      ctrl2 &= 0b01111111;

    Wire.beginTransmission(NAU7802_ADDR);
    Wire.write(REG_CTRL2);
    Wire.write(ctrl2);
    Wire.endTransmission();
  }
}
