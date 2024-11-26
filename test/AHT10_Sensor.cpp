#include <Wire.h>
#include <Arduino.h>

#define AHT_adress 0x39
float Temp; 
float Humi; 

void AHT10_measure(float* temp_val, float* humi_val);

void setup()
{
    Wire.begin();
    Serial.begin(115200);
}

void loop()
{

    AHT10_measure(&Temp, &Humi);
    Serial.print("{AHT10}");
    Serial.print(Temp);
    Serial.print(",");
    Serial.println(Humi);
    delay(200);

}

void AHT10_measure(float* temp_val, float* humi_val)
{
    int data[6];
    Wire.beginTransmission(AHT_adress);
    Wire.endTransmission();
    Wire.requestFrom(AHT_adress, 6);
    while (Wire.available())
    {
        for (int i = 0; i < 6; i++)
        {
            data[i] = Wire.read();
        }
        if (data[0] & 0x08 == 0)
        {
            Wire.beginTransmission(AHT_adress);
            Wire.write(0xE1);
            Wire.write(0x08);
            Wire.write(0x00);
            Wire.endTransmission();
        }
        else
        {
            //Serial.println("no need init");
            Wire.beginTransmission(AHT_adress);
            Wire.write(0xAC);
            Wire.write(0x33);
            Wire.write(0x00);
            Wire.endTransmission();
            delayMicroseconds(75);
            if (data[0] & 0x80 == 0)
            {
                //Serial.println("sensor busy");
                // Serial.println(Wire.read());
                delayMicroseconds(75);
            }
            else
            {
                //Serial.println("sensor not busy");
                // Serial.println(Wire.read());
            }
        }
    }
    int humi_raw = ((data[1] << 12) | (data[2] << 4)) | (data[3] >> 4);
    int temp_raw = ((data[3] & 0x0f) << 16) | (data[4] << 8) | data[5];
    *humi_val = (humi_raw / pow(2, 20)) * 100.0;
    *temp_val = (temp_raw / pow(2, 20)) * 200.0 - 50.0;
}
