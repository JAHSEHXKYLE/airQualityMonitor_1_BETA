/*
    代码BUG测试代码，过于臃肿，待整理 2024年11月8日18:31:04 JAHSEHKYLE




*/
#include <Wire.h>
#include <Arduino.h>

#define BME280_ADDRESS 0x76

unsigned long int temp_raw,pres_raw;
signed long int t_fine;

uint16_t dig_T1;
int16_t dig_T2, dig_T3;
uint16_t dig_P1;
int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

uint8_t samplingFrequency = 0;

signed long int calibration_T(signed long int adc_T);
unsigned long int calibration_P(signed long int adc_P);
void readData();
void writeReg(uint8_t reg_address, uint8_t data);
void readTrim();

void setup()
{
    
    uint8_t osrs_t = 5;             //Temperature oversampling x 16     20 bit / 0.0003 °C
    uint8_t osrs_p = 5;             //Pressure oversampling x 16     20 bit / 0.16 Pa
    uint8_t mode = 3;               //Normal mode
    uint8_t t_sb = 1;               //Tstandby 62.5ms
    uint8_t filter = 2;             //Filter on 0.082 x ODR  Filter打开后只能在Normal模式下使用
    uint8_t spi3w_en = 0;           //3-wire SPI Disable
    uint8_t ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;
    uint8_t config_reg    = (t_sb << 5) | (filter << 2) | spi3w_en;
    Serial.begin(115200);
    Serial.println("****************************BME280 Sensor Test**************************** 002");
    Wire.begin();
    // I2C 总线复位（伪代码）
    for (int i = 0; i < 10; i++) {
        digitalWrite(SDA, LOW);
        delayMicroseconds(10);
        digitalWrite(SDA, HIGH);
        delayMicroseconds(10);
    }

    //writeReg(0xE0,0xB6);
    writeReg(0xF4,ctrl_meas_reg);
    //writeReg(0xF5,config_reg);
    delay(100);
    //writeReg(0x40, 0xBA);
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.write(0xF3);
    Wire.endTransmission();
    Wire.requestFrom(BME280_ADDRESS,1);
    uint8_t status3 = Wire.read();
    Serial.print("0xF3: ");
    Serial.print(status3);
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.write(0x9E);
    Wire.endTransmission();
    Wire.requestFrom(BME280_ADDRESS,1);
    uint8_t status1 = Wire.read();
    Serial.print("0x9E: ");
    Serial.print(status1);
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.write(0x9F);
    Wire.endTransmission();
    Wire.requestFrom(BME280_ADDRESS,1);
    uint8_t status2 = Wire.read();
    Serial.print(", 0x9F: ");
    Serial.print(status2);

    //readTrim();
}

void loop()
{
    /* double temp_act = 0.0, press_act = 0.0;
    signed long int temp_cal;
    unsigned long int press_cal;
    readData();
    temp_cal = calibration_T(temp_raw);
    press_cal = calibration_P(pres_raw);
    temp_act = (double)temp_cal / 100.0;
    press_act = (double)press_cal / 100.0;
    Serial.print("{PRESS}");
    Serial.print(temp_raw);
    Serial.print(",");
    Serial.print(pres_raw);
    Serial.print(",");
    Serial.print(temp_act);
    Serial.print(",");
    Serial.println(press_act);
    delay(100);
    
    samplingFrequency++;
    while(samplingFrequency == 10);  */   
}

void readTrim()
{
    uint8_t data[24], i=0, err, retrytimes = 0;
    for (; i < 24; i++)
    {
        Wire.beginTransmission(BME280_ADDRESS);
        Wire.write(136 + i);
        err = Wire.endTransmission(false);
        if (err!= 0){
            Serial.println("[readTrim()_endTransmission()_001]: Error code: " + String(err) + "From: " + String(i));
            i--;    //重试对本寄存器的读取
            retrytimes++;   //记录重试次数
            Serial.println("    retrytimes: "+ String(retrytimes)); //返回重试次数
            if(retrytimes > 3){
                Serial.println("[readTrim()_endTransmission()_001]: Error message: TimeOut_retrytimes > 3"); //超时重试次数
                retrytimes = 0;   //重置重试次数
                i++;    //跳过本寄存器的读取，继续读取下一个寄存器
            }
        }else{
            Wire.requestFrom(BME280_ADDRESS,1);
            if (Wire.available() == 1){
                data[i] = Wire.read();
                Serial.println("Trim data["+String(i)+"]:      " + String(data[i]));
            }else{
                Serial.print("[readTrim()_requestFrom()_001]: Error message: data size != 1");
                i--;    //重试对本寄存器的读取
                retrytimes++;   //记录重试次数
                Serial.println("    retrytimes: "+ String(retrytimes)); //返回重试次数
                if(retrytimes > 3){
                    Serial.println("[readTrim()_requestFrom()_001]: Error message: TimeOut_retrytimes > 3"); //超时重试次数
                    retrytimes = 0;   //重置重试次数
                    i++;    //跳过本寄存器的读取，继续读取下一个寄存器
                }
            }
        }
    }

    dig_T1 = (data[1] << 8) | data[0];
    dig_T2 = (data[3] << 8) | data[2];
    dig_T3 = (data[5] << 8) | data[4];
    dig_P1 = (data[7] << 8) | data[6];
    dig_P2 = (data[9] << 8) | data[8];
    dig_P3 = (data[11]<< 8) | data[10];
    dig_P4 = (data[13]<< 8) | data[12];
    dig_P5 = (data[15]<< 8) | data[14];
    dig_P6 = (data[17]<< 8) | data[16];
    dig_P7 = (data[19]<< 8) | data[18];
    dig_P8 = (data[21]<< 8) | data[20];
    dig_P9 = (data[23]<< 8) | data[22];

    Serial.print(dig_T1);
    Serial.print(",");
    Serial.print(dig_T2);
    Serial.print(",");
    Serial.print(dig_T3);
    Serial.print(",");
    Serial.print(dig_P1);
    Serial.print(",");
    Serial.print(dig_P2);
    Serial.print(",");
    Serial.print(dig_P3);
    Serial.print(",");
    Serial.print(dig_P4);
    Serial.print(",");
    Serial.print(dig_P5);
    Serial.print(",");
    Serial.print(dig_P6);
    Serial.print(",");
    Serial.print(dig_P7);
    Serial.print(",");
    Serial.print(dig_P8);
    Serial.print(",");
    Serial.print(dig_P9);
    Serial.println();
    /* uint8_t data[24], i=0, err;
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.write(0x88);
    delayMicroseconds(1000);
    err = Wire.endTransmission(false);
    delayMicroseconds(1000);
    if (err!= 0){
        Serial.print("[readTrim()75 ERROR]: ");
        Serial.println(err);
        readTrim(); //如果报错就重新读取
    }else{
        Wire.requestFrom(BME280_ADDRESS,12);
        delayMicroseconds(1000);
        Serial.print("Trim data:");
        Serial.print(Wire.available());
        Serial.print("           ");
        if (Wire.available() == 12){
            while(Wire.available()){
                data[i] = Wire.read();
                i++;
            }
            dig_T1 = (data[1] << 8) | data[0];
            dig_T2 = (data[3] << 8) | data[2];
            dig_T3 = (data[5] << 8) | data[4];
            dig_P1 = (data[7] << 8) | data[6];
            dig_P2 = (data[9] << 8) | data[8];
            dig_P3 = (data[11]<< 8) | data[10];


            dig_P4 = (data[13]<< 8) | data[12];
            dig_P5 = (data[15]<< 8) | data[14];
            dig_P6 = (data[17]<< 8) | data[16];
            dig_P7 = (data[19]<< 8) | data[18];
            dig_P8 = (data[21]<< 8) | data[20];
            dig_P9 = (data[23]<< 8) | data[22];

            Serial.print(dig_T1);
            Serial.print(",");
            Serial.print(dig_T2);
            Serial.print(",");
            Serial.print(dig_T3);
            Serial.print(",");
            Serial.print(dig_P1);
            Serial.print(",");
            Serial.print(dig_P2);
            Serial.print(",");
            Serial.print(dig_P3);
            Serial.print(",");
            Serial.print(dig_P4);
            Serial.print(",");
            Serial.print(dig_P5);
            Serial.print(",");
            Serial.print(dig_P6);
            Serial.print(",");
            Serial.print(dig_P7);
            Serial.print(",");
            Serial.print(dig_P8);
            Serial.print(",");
            Serial.print(dig_P9);
            Serial.println();
        }else{
            Serial.println("[ERROR 140]data size != 24");
            readTrim();
        }
    } */
}

void writeReg(uint8_t reg_address, uint8_t data)
{
    int err;
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.write(reg_address);
    Wire.write(data);
    err = Wire.endTransmission();
    if (err != 0){
        Serial.print("[WriteReg()145 ERROR]:!!endTransmission() error: ");
        Serial.println(err);
        // 这里可以增加一个重试机制，例如重试3次
        for (int retries = 0; retries < 3; retries++) {
            Serial.println("Retrying...");
            Wire.beginTransmission(BME280_ADDRESS);
            Wire.write(reg_address);
            Wire.write(data);
            err = Wire.endTransmission();
            if (err == 0){
                return; // 成功时返回
            }
        }
        
        // 如果多次重试失败，给出提示并停止执行
        Serial.println("Failed to communicate with sensor after multiple attempts.");
        while (1);
    }
}



void readData(){    //读取校准前的数据
    int i = 0, err;
    uint32_t data[6];
    Wire.beginTransmission(BME280_ADDRESS);
    Wire.write(0xF7);
    err = Wire.endTransmission();
    if (err!= 0){
        Serial.print("[readData()178 ERROR]:!!endTransmission() error: ");
        Serial.println(err);

        for (int i = 0; i < 3; i++){
            Serial.println("Retrying...");
            Wire.beginTransmission(BME280_ADDRESS);
            Wire.write(0xF7);
            err = Wire.endTransmission();
            if (err == 0){
                break;
            }
        }
    }else{
        Wire.requestFrom(BME280_ADDRESS,6);
        if (Wire.available() == 6){
            while(Wire.available()){
                data[i] = Wire.read();
                i++;
            }
            pres_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
            temp_raw = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
        }else{
            Serial.println("[ERROR 215]data size != 6");
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
