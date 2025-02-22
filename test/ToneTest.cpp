#include <Arduino.h>

#define BUZZER_PIN 13 // 蜂鸣器连接的引脚

// 五音的频率（单位：Hz）
int tones[] = {2100, 5210}; // C4, D4, E4, F4, G4

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
    tone(BUZZER_PIN, tones[0], 110);
    delay(210);
    tone(BUZZER_PIN, tones[1], 210);
    delay(5000);
    for (int i = 0; i < 5; i++){
        tone(BUZZER_PIN, tones[0], 500);
        delay(1000);
    }
    int tone_val = 5210;
    for (int i = 0; i < 5; i++){
        tone(BUZZER_PIN, tone_val, 100);
        delay(200);
        tone(BUZZER_PIN, tone_val, 100);
        delay(1500);
    }
}