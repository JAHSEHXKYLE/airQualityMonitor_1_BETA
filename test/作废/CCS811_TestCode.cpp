#include <Wire.h>
#include <Arduino.h>
#include <pins_arduino.h>

#define CCS811_ADDR 0x5A  // CCS811 的 I²C 地址

byte status;
byte error;
uint16_t eCO2;
uint16_t eTVOC;

bool ccs811_init();
bool ccs811_read_status();
void ccs811_read_error();
bool ccs811_read_data();

void setup() {
  Serial.begin(115200);
  Wire.begin();  // 初始化 I²C 总线

  // 初始化 CCS811 传感器
  if (!ccs811_init()) {
    Serial.println("CCS811 初始化失败");
    while (1);
  }

  Serial.println("CCS811 初始化成功");
}

void loop() {
  // 读取 CCS811 数据
  if (ccs811_read_data()) {
    Serial.print("eCO2: ");
    Serial.print(eCO2);
    Serial.print(" ppm  eTVOC: ");
    Serial.print(eTVOC);
    Serial.println(" ppb");
  } else {
    Serial.println("读取 CCS811 数据失败");
  }

  delay(1000);  // 每秒读取一次
}

// 初始化 CCS811 传感器
bool ccs811_init() {
  // 写入应用程序启动命令
  Wire.beginTransmission(CCS811_ADDR);
  Wire.write(0xF4);  // APP_START 命令
  Wire.endTransmission();

  delay(100);  // 等待传感器启动

  // 检查状态寄存器
  if (!ccs811_read_status()) {
    return false;
  }

  // 配置测量模式
  byte mode = 0x01;  // Mode 1：恒定功率模式，每秒测量一次
  Wire.beginTransmission(CCS811_ADDR);
  Wire.write(0x01);  // 写入测量模式寄存器地址
  Wire.write(mode);  // 写入测量模式
  Wire.endTransmission();

  return true;
}

// 读取 CCS811 状态寄存器
bool ccs811_read_status() {
  Wire.beginTransmission(CCS811_ADDR);
  Wire.write(0x00);  // 状态寄存器地址
  Wire.endTransmission();

  Wire.requestFrom(CCS811_ADDR, 1);
  if (Wire.available()) {
    status = Wire.read();
  } else {
    return false;
  }

  // 检查错误标志
  if (status & 0x01) {
    ccs811_read_error();
  }

  return true;
}

// 读取 CCS811 错误寄存器
void ccs811_read_error() {
  Wire.beginTransmission(CCS811_ADDR);
  Wire.write(0xE0);  // 错误寄存器地址
  Wire.endTransmission();

  Wire.requestFrom(CCS811_ADDR, 1);
  if (Wire.available()) {
    error = Wire.read();
    Serial.print("错误码: ");
    Serial.println(error, HEX);
  }
}

// 读取 CCS811 数据
bool ccs811_read_data() {
  Wire.beginTransmission(CCS811_ADDR);
  Wire.write(0x02);  // 算法结果数据寄存器地址
  Wire.endTransmission();

  Wire.requestFrom(CCS811_ADDR, 8);

  if (Wire.available() >= 8) {
    uint8_t data[8];
    for (int i = 0; i < 8; i++) {
      data[i] = Wire.read();
    }

    eCO2 = (data[0] << 8) | data[1];
    eTVOC = (data[2] << 8) | data[3];

    // 检查数据有效性
    if (eCO2 >= 400 && eCO2 <= 29206 && eTVOC <= 32768) {
      return true;
    }
  }

  return true;
}