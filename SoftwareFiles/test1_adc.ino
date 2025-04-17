#include <Wire.h>
#include <Adafruit_NAU7802.h>

// Create an instance of the NAU7802 object.
Adafruit_NAU7802 nau;

void setup()
{
  Serial.begin(115200);
  // Allow time for the serial monitor to initialize.
  delay(1000);

  // Initialize I²C on ESP32:
  // SDA is mapped to GPIO17 (IO17) and SCL is mapped to GPIO16 (IO16).
  //  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.begin(17, 16);
  Serial.println("I2C initialized on SDA=17 and SCL=16");

  // Set up the ADC Data Ready (DRDY) pin.
  // The NAU7802 ADC DRDY (pin 12) is mapped to ESP32 GPIO21 (IO21).
  pinMode(21, INPUT);
  Serial.println("DRDY pin set up on GPIO21");

  // Initialize the NAU7802 sensor.
  Serial.println("Initializing NAU7802...");
  if (!nau.begin())
  {
    Serial.println("Failed to find NAU7802");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("NAU7802 found");

  // Set the LDO (Low Drop-Out regulator) voltage to 3.0V.
  nau.setLDO(NAU7802_3V3);
  Serial.print("LDO voltage set to ");
  switch (nau.getLDO())
  {
  case NAU7802_4V5:
    Serial.println("4.5V");
    break;
  case NAU7802_4V2:
    Serial.println("4.2V");
    break;
  case NAU7802_3V9:
    Serial.println("3.9V");
    break;
  case NAU7802_3V6:
    Serial.println("3.6V");
    break;
  case NAU7802_3V3:
    Serial.println("3.3V");
    break;
  case NAU7802_3V0:
    Serial.println("3.0V");
    break;
  case NAU7802_2V7:
    Serial.println("2.7V");
    break;
  case NAU7802_2V4:
    Serial.println("2.4V");
    break;
  case NAU7802_EXTERNAL:
    Serial.println("External");
    break;
  default:
    Serial.println("Unknown");
    break;
  }

  // Set gain to 128x.
  nau.setGain(NAU7802_GAIN_128);
  Serial.print("Gain set to ");
  switch (nau.getGain())
  {
  case NAU7802_GAIN_1:
    Serial.println("1x");
    break;
  case NAU7802_GAIN_2:
    Serial.println("2x");
    break;
  case NAU7802_GAIN_4:
    Serial.println("4x");
    break;
  case NAU7802_GAIN_8:
    Serial.println("8x");
    break;
  case NAU7802_GAIN_16:
    Serial.println("16x");
    break;
  case NAU7802_GAIN_32:
    Serial.println("32x");
    break;
  case NAU7802_GAIN_64:
    Serial.println("64x");
    break;
  case NAU7802_GAIN_128:
    Serial.println("128x");
    break;
  default:
    Serial.println("Unknown Gain");
    break;
  }

  // Set the conversion rate to 10 samples per second.
  nau.setRate(NAU7802_RATE_10SPS);
  Serial.print("Conversion rate set to ");
  switch (nau.getRate())
  {
  case NAU7802_RATE_10SPS:
    Serial.println("10 SPS");
    break;
  case NAU7802_RATE_20SPS:
    Serial.println("20 SPS");
    break;
  case NAU7802_RATE_40SPS:
    Serial.println("40 SPS");
    break;
  case NAU7802_RATE_80SPS:
    Serial.println("80 SPS");
    break;
  case NAU7802_RATE_320SPS:
    Serial.println("320 SPS");
    break;
  default:
    Serial.println("Unknown Rate");
    break;
  }

  // Flush out 10 initial readings to allow the sensor to stabilize.
  for (uint8_t i = 0; i < 10; i++)
  {
    while (!nau.available())
    {
      delay(1);
    }
    nau.read();
  }

  // Calibrate the internal offset.
  while (!nau.calibrate(NAU7802_CALMOD_INTERNAL))
  {
    Serial.println("Failed to calibrate internal offset, retrying!");
    delay(1000);
  }
  Serial.println("Calibrated internal offset");

  // Calibrate the system offset.
  while (!nau.calibrate(NAU7802_CALMOD_OFFSET))
  {
    Serial.println("Failed to calibrate system offset, retrying!");
    delay(1000);
  }
  Serial.println("Calibrated system offset");
}

void loop()
{
  // Wait until a new conversion is available.
  while (!nau.available())
  {
    delay(1);
  }

  // Read and print the conversion result.
  int32_t val = nau.read();
  Serial.print("Read value: ");
  Serial.println(val);
}
