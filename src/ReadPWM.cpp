/* CO2传感器测试代码 */

#include <Arduino.h>

#define interruptPin 6
float pulseWidth = 0;
bool LEDState = false;
uint16_t Sensorval = false;

void GetInterrupt(float *WidthVal);
void FlashLED();

void setup() {
    Serial.begin(115200);
    pinMode(interruptPin, INPUT);
    pinMode(13, OUTPUT);
    pinMode(12, OUTPUT);
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
}
 
void loop() {
    FlashLED();
    //Sensorval = digitalRead(interruptPin);
    //Serial.println(Sensorval); // 打印传感器状态
    GetInterrupt(&pulseWidth);
    Serial.println(pulseWidth); // 打印脉冲宽度
    //delay(100);
}
 
void GetInterrupt(float *WidthVal)
{
    unsigned long LOWvalue = 0;
    unsigned long HIGHvalue = 0;
    if (digitalRead(interruptPin) == HIGH) {
        LOWvalue = micros(); // 获取当前时间
        while (digitalRead(interruptPin) == HIGH) {
            LOWvalue = micros(); // 获取当前时间
        }
        while (digitalRead(interruptPin) == LOW){
            HIGHvalue = micros(); // 获取当前时间
        }
        Serial.println(LOWvalue); // 打印低电平时间
        Serial.println(HIGHvalue); // 打印高电平时间
        *WidthVal = (((1004000 - (HIGHvalue - LOWvalue)) / 1000)-2)*5; // 计算脉冲宽度
    }
    
}

void FlashLED() {
  digitalWrite(13, LEDState);
  LEDState =!LEDState;
  digitalWrite(12, LEDState);
}