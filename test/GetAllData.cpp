#include <Arduino.h>
#include <Wire.h>

#define DEVICE_ADDR 0x12 // 假设设备I2C地址为0x12

void setup() {
  Serial.begin(115200);
  Wire.begin();
}

uint8_t readRegister(uint16_t regAddr);

void loop() {
  for (uint16_t regAddr = 0x00; regAddr <= 0xFF; regAddr++) { // 8位地址遍历
    uint8_t data = readRegister(regAddr);
    if (data != 0xFF) { // 过滤无效响应（可选）
      Serial.print("Reg 0x");
      Serial.print(regAddr, HEX);
      Serial.print(": 0x");
      Serial.println(data, HEX);
    }
    delay(10); // 防止总线拥堵
  }
  while(1); // 遍历完成后停止
}

uint8_t readRegister(uint16_t regAddr) {
  Wire.beginTransmission(DEVICE_ADDR);
  Wire.write(regAddr); // 发送要读取的寄存器地址（仅8位地址）
  if (Wire.endTransmission(false) != 0) { // 保持连接（不发送停止位）
    return 0xFF; // 通信失败
  }
  Wire.requestFrom(DEVICE_ADDR, 1);
  return Wire.read();
}