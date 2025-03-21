#include <Arduino.h>
#include <Wire.h>

#define BMP280_ADDR 0x76 // BMP280 I2C address
#define REG_9E 0x9E      // Target register 0x9E
#define REG_9F 0x9F      // Target register 0x9F

// Function declarations
uint8_t readBMP280Register(uint8_t reg);

void setup();
void loop();

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Initialize I2C
}

void loop() {
  // Read BMP280 register 0x9E
  uint8_t value_9E = readBMP280Register(REG_9E);
  Serial.print("Register 0x9E value: 0x");
  Serial.println(value_9E, HEX);

  // Read BMP280 register 0x9F
  uint8_t value_9F = readBMP280Register(REG_9F);
  Serial.print("Register 0x9F value: 0x");
  Serial.println(value_9F, HEX);

  delay(2000); // Read every 2 seconds
}

// Function to read BMP280 register
uint8_t readBMP280Register(uint8_t reg) {
  Wire.beginTransmission(BMP280_ADDR);
  Wire.write(reg); // Send register address
  Wire.endTransmission();

  Wire.requestFrom(BMP280_ADDR, 1); // Request to read 1 byte
  if (Wire.available()) {
    return Wire.read(); // Return the read value
  }
  return 0; // Return 0 if reading failed
}
