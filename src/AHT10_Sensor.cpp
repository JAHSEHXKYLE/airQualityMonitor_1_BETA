#include <Wire.h>
#include <Arduino.h>

#define AHT_adress 0x38
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
    int data[6] = {0};
    Wire.beginTransmission(AHT_adress);
    Wire.write(0xAC); // 温度测量
    Wire.write(0x33);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(AHT_adress, 6);
    while (Wire.available())
    {
        for (int i = 0; i < 6; i++)
        {
            data[i] = Wire.read();
           
        }
        Serial.print(data[0]);
        // if (data[0] && 0x08 == 0) // 检测校准使能位（为了兼容旧版本的AHT10）
        // {
        //     Wire.beginTransmission(AHT_adress);
        //     Wire.write(0xE1); // 初始化
        //     // Wire.write(0x08); // 
        //     // Wire.write(0x00);
        //     Wire.endTransmission();
        //     Serial.println("init AHT10");
        // }
        // else
        // {
        //     //Serial.println("no need init");
        //     Wire.beginTransmission(AHT_adress); // 读取数据
        //     Wire.write(0xAC); // 温度测量
        //     Wire.write(0x33);
        //     Wire.write(0x00);
        //     Wire.endTransmission();
        //     delayMicroseconds(75);
        //     if (data[0] && 0x80 == 0) // 判断设备是否处于测量忙状态
        //     {
        //         //Serial.println("sensor busy");
        //         // Serial.println(Wire.read());
        //         delayMicroseconds(75);
        //     }
        //     else
        //     {
        //         //Serial.println("sensor not busy");
        //         // Serial.println(Wire.read());
        //     }
        // }
    }
    int humi_raw = ((data[1] << 12) | (data[2] << 4)) | (data[3] >> 4);
    int temp_raw = ((data[3] & 0x0f) << 16) | (data[4] << 8) | data[5];
    *humi_val = (humi_raw / pow(2, 20)) * 100.0;
    *temp_val = (temp_raw / pow(2, 20)) * 200.0 - 50.0;
    
}
