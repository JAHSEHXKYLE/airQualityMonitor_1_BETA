/* 合宙esp32c3测试代码 */

#include <Arduino.h>

#define LED_BUILTIN1 12
#define LED_BUILTIN2 13

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN1, OUTPUT);
  pinMode(LED_BUILTIN2, OUTPUT);
  digitalWrite(LED_BUILTIN1, LOW);
  digitalWrite(LED_BUILTIN2, LOW);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN1, LOW);
  digitalWrite(LED_BUILTIN2, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN1, HIGH);
  digitalWrite(LED_BUILTIN2, HIGH);
  delay(1000);
  Serial.println("Hello, ESP32C3");
}
