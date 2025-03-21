#include <Wire.h>
#include <Arduino.h>

#define BMP_ADDR 0x76 // I2C地址 SDO引脚低电平为0x76 高电平为0x77
#define AHT10_ADDR 0x38 // I2C地址

double temp_act = 0.0, press_act = 0.0; //定义温度、气压的精确值
unsigned long int temp_raw,pres_raw;
signed long int t_fine;
uint16_t dig_T1, dig_P1;    //BMP180原始数据
int16_t dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9; //BMP180校准系数

float Temp = 0.0; 
float Humi = 0.0; 
/***************************************温湿传感器 ↓**************************************/
void AHT10_measure(float* temp_val, float* humi_val);
/***************************************温湿传感器 ↑**************************************/

/***************************************气压传感器 ↓**************************************/
void BMPReadTrim(); //气压校准系数读取
void BMPinit(); //气压传感器初始化
void BMPWriteReg(uint8_t reg, uint8_t data);    //BMP280寄存器写入
void BMPGetData(double* P_var, double* T_var); //获取BMP280数据
unsigned long int calibration_P(signed long int adc_P); //气压校准算法
signed long int calibration_T(signed long int adc_T);   //温度校准算法
/***************************************气压传感器 ↑**************************************/

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    Serial.println(">>>BMP280 and AHT10 Sensor Test<<<");
    BMPinit();
    BMPReadTrim();
}

void loop()
{
    BMPGetData(&press_act, &temp_act);
    AHT10_measure(&Temp, &Humi);
    // Serial.print("{SensorData}");
    Serial.print("/*");
    Serial.print(Temp);
    Serial.print(",");
    Serial.print(Humi);
    Serial.print(",");
    Serial.print(press_act);
    Serial.print(",");
    Serial.print(temp_act);
    Serial.println("*/");
    delay(100);
    while(1);
}

void AHT10_measure(float* temp_val, float* humi_val)
{
    int data[6], err;
    Wire.beginTransmission(AHT10_ADDR);
    err = Wire.endTransmission();
    if (err!= 0){
        Serial.print("[err 101]: endTransmission failed with error code: ");
        switch (err)
        {
        case 1:
            Serial.println("1");
            break;
        
        case 2:
            Serial.println("2");
            break;
        case 3:
            Serial.println("3");
            break;
        case 4:
            Serial.println("4");
            break;
        }
    }
    else{
        Wire.requestFrom(AHT10_ADDR, 6, 1);
        if(Wire.available() == 6)
        {
            for (int i = 0; i < 6; i++)
            {
                data[i] = Wire.read();
            }
            if ((data[0] & 0x08) == 0)
            {
                Wire.beginTransmission(AHT10_ADDR);
                Wire.write(0xE1);
                Wire.write(0x08);
                Wire.write(0x00);
                Wire.endTransmission();
            }
            else
            {
                //Serial.println("no need init");
                Wire.beginTransmission(AHT10_ADDR);
                Wire.write(0xAC);
                Wire.write(0x33);
                Wire.write(0x00);
                Wire.endTransmission();
                delayMicroseconds(75);
                if (data[0] & 0x80 == 0)
                {
                    //Serial.println("sensor busy");
                    // Serial.println(Wire.read());
                    delayMicroseconds(75);
                }
                else
                {
                    //Serial.println("sensor not busy");
                    // Serial.println(Wire.read());
                }
            }
            int humi_raw = ((data[1] << 12) | (data[2] << 4)) | (data[3] >> 4);
            int temp_raw = ((data[3] & 0x0f) << 16) | (data[4] << 8) | data[5];
            *humi_val = (humi_raw / pow(2, 20)) * 100.0;
            *temp_val = (temp_raw / pow(2, 20)) * 200.0 - 50.0;
        }else{
            Serial.println("Error000: not enough data received from AHT10");
        }
    }
}

void BMPReadTrim(){
    uint8_t data[24], i = 0, err;
    Wire.beginTransmission(BMP_ADDR);
    Wire.write(0x88);
    err = Wire.endTransmission();
    if (err!= 0){
        Serial.print("[err 102]: endTransmission failed with error code: ");
        switch (err)
        {
        case 1:
            Serial.println("1");
            break;
        
        case 2:
            Serial.println("2");
            break;
        case 3:
            Serial.println("3");
            break;
        case 4:
            Serial.println("4");
            break;
        }
        Wire.requestFrom(BMP_ADDR, 24);
        if (Wire.available() == 24)
        {
            while (Wire.available()) {
            data[i] = Wire.read();
            i++;
            }
            dig_T1 = (data[1] << 8) | data[0];
            dig_T2 = (data[3] << 8) | data[2];
            dig_T3 = (data[5] << 8) | data[4];
            dig_P1 = (data[7] << 8) | data[6];
            dig_P2 = (data[9] << 8) | data[8];
            dig_P3 = (data[11] << 8) | data[10];
            dig_P4 = (data[13] << 8) | data[12];
            dig_P5 = (data[15] << 8) | data[14];
            dig_P6 = (data[17] << 8) | data[16];
            dig_P7 = (data[19] << 8) | data[18];
            dig_P8 = (data[21] << 8) | data[20];
            dig_P9 = (data[23] << 8) | data[22];
        }else{
            Serial.println("Error001: not enough data received from BMP280");
            Serial.println(Wire.available());
        }
    }
}

void BMPinit(){
    uint8_t osrs_t = 5;             //Temperature oversampling x 16     20 bit / 0.0003 °C
    uint8_t osrs_p = 5;             //Pressure oversampling x 16     20 bit / 0.16 Pa
    uint8_t mode = 3;               //Normal mode
    uint8_t t_sb = 1;               //Tstandby 62.5ms
    uint8_t filter = 2;             //Filter on 0.082 x ODR  Filter打开后只能在Normal模式下使用
    uint8_t spi3w_en = 0;           //3-wire SPI Disable

    uint8_t ctrl_mesa_reg = (osrs_t << 5) | (osrs_p << 2) | mode;
    uint8_t config_reg = (t_sb << 5) | (filter << 2) | spi3w_en;

    BMPWriteReg(0xE0, 0xB6); // 传感器config复位
    BMPWriteReg(0xF4, ctrl_mesa_reg); // 开启温度高精度模式、大气采样高精度模式、设置传感器为Normal mode
    BMPWriteReg(0xF5, config_reg); 
}

void BMPWriteReg(uint8_t reg, uint8_t data){
    Wire.beginTransmission(BMP_ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();    
}

void BMPGetData(double* P_var, double* T_var){ //获取BMP280数据
    int i = 0, err;
    uint8_t data[6];
    Wire.beginTransmission(BMP_ADDR);
    Wire.write(0xF7);
    err = Wire.endTransmission();
    if (err!= 0){
        Serial.print("[err 103]: endTransmission failed with error code: ");
        switch (err)
        {
        case 1:
            Serial.println("1");
            break;
        
        case 2:
            Serial.println("2");
            break;
        case 3:
            Serial.println("3");
            break;
        case 4:
            Serial.println("4");
            break;
        }
        Wire.requestFrom(BMP_ADDR, 6);
        Serial.println(Wire.available());
        if (Wire.available() == 6)
        {
            while (Wire.available()) {
            data[i] = Wire.read();
            Serial.println(data[i]);
            i++;
            }
            pres_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);   //读取原始数据
            temp_raw = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
            *P_var = calibration_P(pres_raw)/100.00;
            *T_var = calibration_T(temp_raw)/100.00;
        }else{
            Serial.println("Error002: not enough data received from BMP280");
        }
    }
}

signed long int calibration_T(signed long int adc_T)
{
    signed long int var1, var2, T;
    var1 = ((((adc_T >> 3) - ((signed long int)dig_T1<<1))) * ((signed long int)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((signed long int)dig_T1)) * ((adc_T>>4) - ((signed long int)dig_T1))) >> 12) * ((signed long int)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

unsigned long int calibration_P(signed long int adc_P)
{
    signed long int var1, var2, valt1, valt2, T;
    unsigned long int P;
    var1 = (((signed long int)t_fine)>>1) - (signed long int)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11) * ((signed long int)dig_P6);
    var2 = var2 + ((var1*((signed long int)dig_P5))<<1);
    var2 = (var2>>2)+(((signed long int)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2)*(var1>>2)) >> 13)) >>3) + ((((signed long int)dig_P2) * var1)>>1))>>18;
    var1 = ((((32768+var1))*((signed long int)dig_P1))>>15);
    if (var1 == 0){
        return 0;
    }    
    P = (((unsigned long int)(((signed long int)1048576)-adc_P)-(var2>>12)))*3125;
    if(P<0x80000000){
       P = (P << 1) / ((unsigned long int) var1);   
    }
    else{                  
        P = (P / (unsigned long int)var1) * 2;    
    }
    var1 = (((signed long int)dig_P9) * ((signed long int)(((P>>3) * (P>>3))>>13)))>>12;
    var2 = (((signed long int)(P>>2)) * ((signed long int)dig_P8))>>13;
    P = (unsigned long int)((signed long int)P + ((var1 + var2 + dig_P7) >> 4));
    return P;
}
