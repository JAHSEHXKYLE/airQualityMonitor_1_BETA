#include <Arduino.h>
#include <Wire.h>

#define BMP_ADDR 0x76 // I2C地址 SDO引脚低电平为0x76 高电平为0x77

double temp_act = 0.0, press_act = 0.0; //定义温度、气压的精确值
unsigned long int temp_raw,pres_raw;
signed long int t_fine;
const uint16_t dig_T1 = 27530, 
            dig_P1 = 36653;    //BMP280校准系数存储
const int16_t dig_T2 = 26600, 
            dig_T3 = -1000, 
            dig_P2 = -10718, 
            dig_P3 = 3024, 
            dig_P4 = -1071, 
            dig_P5 = 353, 
            dig_P6 = -7, 
            dig_P7 = 15500, 
            dig_P8 = -14600, 
            dig_P9 = 6000; //BMP180校准系数存储

void BMPinit(); //气压传感器初始化
void BMPWriteReg(uint8_t reg, uint8_t data);    //BMP280寄存器写入
void BMPGetData(double* P_var, double* T_var); //获取BMP280数据
unsigned long int calibration_P(signed long int adc_P); //气压校准算法
signed long int calibration_T(signed long int adc_T);   //温度校准算法

