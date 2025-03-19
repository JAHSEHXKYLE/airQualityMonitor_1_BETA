/******************************************************************************
  * 测试硬件：ColorEasyDuino开发板
  * 版 本 号: V1.0
  * 修改作者: www.lckfb.com
  * 修改日期: 2024年04月11日
  * 功能介绍:
  *****************************************************************************
  * 开发板软硬件资料与相关项目软硬件资料官网全部开源
  * 开发板官网：www.lckfb.com
  * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
  * 立创论坛：club.szlcsc.com
  * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
  * 不靠卖板赚钱，以培养中国工程师为己任
******************************************************************************/
#include <Wire.h>
#include <Arduino.h>

#define DEVICE_ADDRESS  0X1A
#define DATA_ADDRESS    0X00
void setup() {
  // 初始化串行通信
  Serial.begin(9600);
  // 初始化I2C通信
  Wire.begin();
}

void loop() {
  // 发起I2C读取传感器数据的请求
  Wire.beginTransmission(DEVICE_ADDRESS); // AGS01DB的I2C地址，可能需要根据实际情况调整
  Wire.write(byte(DATA_ADDRESS));
  if (Wire.endTransmission() != 0) Serial.println("No sensor was detected"); // 检查ACK，非0值表示出错
  Wire.endTransmission(); // 结束传输，准备读取数据

  // 读取数据
  Wire.requestFrom(DEVICE_ADDRESS, 5); // 请求2字节长度的数据

  if(Wire.available() == 5) {
    byte data1 = Wire.read(); // 读取第一个字节
    byte data2 = Wire.read(); // 读取第二个字节
    byte data3 = Wire.read(); // 读取第三个字节
    byte data4 = Wire.read(); // 读取第四个字节
    byte data5 = Wire.read(); // 读取第五个字节

    // 处理和组合数据
    int gasConcentration = (data2 << 16) | (data3<<8) | data4;
    // 打印结果
    Serial.print("TVOC气体浓度: ");
    Serial.println(gasConcentration);
  }
  delay(1000); // 稍作延时再次读取
}