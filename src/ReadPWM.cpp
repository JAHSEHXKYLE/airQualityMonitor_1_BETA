/* CO2传感器测试代码 */

include <Arduino.h>

#define interruptPin = 11;
volatile unsigned long pulseWidth = 0;

void GetInterrupt(*WidthVal);

void setup() {
  Serial.begin(115200);
  pinMode(interruptPin, INPUT);
}
 
void loop() {
  GetInterrupt(&pulseWidth);
  Serial.println(pulseWidth); // 打印脉冲宽度
  delay(1000);
}
 
void GetInterrupt(*WidthVal)
{
  if (digitalRead(interruptPin) == HIGH) {
    *WidthVal = micros(); // 获取当前时间
    while (digitalRead(interruptPin) == HIGH) {
      *WidthVal = micros(); // 获取当前时间
    }
  }
}