#include <Arduino.h>

#define LED_BUILTIN1 8


void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN1, OUTPUT);
  digitalWrite(LED_BUILTIN1, LOW);

}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN1, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN1, HIGH);
  Serial.println("Hello, ESP32C3");
}
