#include <Arduino.h>
#include <Wire.h>


#define PMS7003_I2C_ADDR 0x12

void readPMS7003Data();

void setup() {
    Serial.begin(115200);
    Wire.begin();
}

void loop() {
    readPMS7003Data();
    delay(1000);
}

void readPMS7003Data() {
    Wire.beginTransmission(PMS7003_I2C_ADDR);
    Wire.write(0x00);
    Wire.endTransmission();
    delay(100);

    Wire.requestFrom(PMS7003_I2C_ADDR, 32);
    if (Wire.available() >= 32) {
        byte buffer[32];
        for (int i = 0; i < 32; i++) {
            buffer[i] = Wire.read();
        }
        unsigned int checksum = buffer[0] | buffer[1] << 8;
        unsigned int pm10_standard = (buffer[5] | buffer[4] << 8);
        unsigned int pm25_standard = (buffer[7] | buffer[6] << 8);
        unsigned int pm100_standard = (buffer[9] | buffer[8] << 8);
        unsigned int pm10_ambient = (buffer[11] | buffer[10] << 8);
        unsigned int pm25_ambient = (buffer[13] | buffer[12] << 8);
        unsigned int pm100_ambient = (buffer[15] | buffer[14] << 8);
        unsigned int particles_03um = (buffer[17] | buffer[16] << 8);
        unsigned int particles_05um = (buffer[19] | buffer[18] << 8);
        unsigned int particles_10um = (buffer[21] | buffer[20] << 8);
        unsigned int particles_25um = (buffer[23] | buffer[22] << 8);
        unsigned int particles_50um = (buffer[25] | buffer[24] << 8);
        unsigned int particles_100um = (buffer[27] | buffer[26] << 8);

        for (int i = 0; i < 32; i++)
        {
            Serial.print(buffer[i], HEX);
            Serial.print(" ");
        }
        
        Serial.println();
        Serial.print("Checksum: ");
        Serial.println(checksum);
        Serial.print("PM1.0 (Standard): ");
        Serial.print(pm10_standard);
        Serial.println(" ug/m3");
        Serial.print("PM2.5 (Standard): ");
        Serial.print(pm25_standard);
        Serial.println(" ug/m3");
        Serial.print("PM10 (Standard): ");
        Serial.print(pm100_standard);
        Serial.println(" ug/m3");

        Serial.print("PM1.0 (Ambient): ");
        Serial.print(pm10_ambient);
        Serial.println(" ug/m3");
        Serial.print("PM2.5 (Ambient): ");
        Serial.print(pm25_ambient);
        Serial.println(" ug/m3");
        Serial.print("PM10 (Ambient): ");
        Serial.print(pm100_ambient);
        Serial.println(" ug/m3");

        Serial.print("Particles 0.3um: ");
        Serial.print(particles_03um);
        Serial.println(" #/0.1L");
        Serial.print("Particles 0.5um: ");
        Serial.print(particles_05um);
        Serial.println(" #/0.1L");
        Serial.print("Particles 1.0um: ");
        Serial.print(particles_10um);
        Serial.println(" #/0.1L");
        Serial.print("Particles 2.5um: ");
        Serial.print(particles_25um);
        Serial.println(" #/0.1L");
        Serial.print("Particles 5.0um: ");
        Serial.print(particles_50um);
        Serial.println(" #/0.1L");
        Serial.print("Particles 10.0um: ");
        Serial.print(particles_100um);
        Serial.println(" #/0.1L");
    } else {
        Serial.println("No data received");
    }
}