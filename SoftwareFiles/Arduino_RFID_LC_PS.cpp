/**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 *
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 *
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * This sample shows how to read and write data blocks on a MIFARE Classic PICC
 * (= card/tag).
 *
 * BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).
 *
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 *
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;


/* 
  IR Breakbeam sensor demo!
*/

#define SENSORPIN 4

// variables will change:
int sensorState = 0, lastState=0;         // variable for reading the pushbutton status

#include <Adafruit_NAU7802.h>

Adafruit_NAU7802 nau;

/**
 * Initialize.
 */
void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    
    //**************************************
		//Start code for RFID
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
    Serial.print(F("Using key (for A and B):"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();

    Serial.println(F("BEWARE: Data will be written to the PICC, in sector #1"));
    
    
    //**************************************
    //Start setup for the infrared presence sensor
    // initialize the LED pin as an output:
	  pinMode(LED_BUILTIN, OUTPUT);      
	  // initialize the sensor pin as an input:
	  pinMode(SENSORPIN, INPUT);     
	  //digitalWrite(SENSORPIN, HIGH); // turn on the pullup //comment this out
	  
	  
	  //**************************************
		//Start code for load cell
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
}

/**
 * Main loop.
 */
void loop() {
		//**************************************
		//Start of infrared Presence sensor
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
		
		//**************************************
		//Start code for load cell
	  while (! nau.available()) {
	    delay(1);
	  }
	  int32_t val = nau.read();
	  Serial.print("Read "); Serial.println(val);
		

		//**************************************
		delay(2000);         // delay to not spam terminal and be able to see what we are doing
		
		//**************************************
		// start of RFID
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }

    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    byte sector         = 1;
    byte blockAddr      = 4;
    byte dataBlock[]    = {
        0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
        0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
        0x09, 0x0a, 0xff, 0x0b, //  9, 10, 255, 11,
        0x0c, 0x0d, 0x0e, 0x0f  // 12, 13, 14, 15
    };
    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();

    // Read data from the block
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();
    Serial.println();

    // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Write data to the block
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.println();

    // Read data from the block (again, should now be what we have written)
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();

    // Check that data in block is what we have written
    // by counting the number of bytes that are equal
    Serial.println(F("Checking result..."));
    byte count = 0;
    for (byte i = 0; i < 16; i++) {
        // Compare buffer (= what we've read) with dataBlock (= what we've written)
        if (buffer[i] == dataBlock[i])
            count++;
    }
    Serial.print(F("Number of bytes that match = ")); Serial.println(count);
    if (count == 16) {
        Serial.println(F("Success :-)"));
    } else {
        Serial.println(F("Failure, no match :-("));
        Serial.println(F("  perhaps the write didn't work properly..."));
    }
    Serial.println();

    // Dump the sector data
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
    
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
