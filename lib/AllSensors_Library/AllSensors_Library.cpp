#include "AllSensors_Library.h"
#include <pins_arduino.h>   // 默认引脚定义

CCS811 ccs811;

signed long int ALL_SENSORS::t_fine;
uint16_t ALL_SENSORS::dig_T1;
int16_t ALL_SENSORS::dig_T2;
int16_t ALL_SENSORS::dig_T3;
uint16_t ALL_SENSORS::dig_P1;
int16_t ALL_SENSORS::dig_P2;
int16_t ALL_SENSORS::dig_P3;
int16_t ALL_SENSORS::dig_P4;
int16_t ALL_SENSORS::dig_P5;
int16_t ALL_SENSORS::dig_P6;
int16_t ALL_SENSORS::dig_P7;
int16_t ALL_SENSORS::dig_P8;
int16_t ALL_SENSORS::dig_P9;
bool ALL_SENSORS::isTestMode = false;

void ALL_SENSORS::setTestMode(bool mode){
    isTestMode = mode;
}

//初始化所有传感器
void ALL_SENSORS::init_all_sensors(){
    Debug_Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);  //初始化I2C总线
    init_BMP280();
    init_SC8();
    init_WZS(Seri1_RX_PIN, Seri1_TX_PIN);
    init_CCS811();
}

void ALL_SENSORS::I2c_Write_Reg(uint8_t I2c_address, uint8_t reg_address, uint8_t data) {
    Wire.beginTransmission(I2c_address);
    Wire.write(reg_address);
    Wire.write(data);
    delayMicroseconds(50);
    Wire.endTransmission();    
}

/********************************************************************************************************************************/
// BMP280代码

// 初始化BMP280
uint8_t ALL_SENSORS::init_BMP280(){
    uint8_t osrs_t = 2;             //Temperature oversampling x 2
    uint8_t osrs_p = 5;             //Pressure oversampling x 16
    uint8_t mode = 3;               //Normal mode
    uint8_t t_sb = 3;               //Tstandby 250ms
    uint8_t filter = 4;             //Filter 0.021 x ODR
    uint8_t spi3w_en = 0;           //3-wire SPI Disable
    
    uint8_t ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;
    uint8_t config_reg    = (t_sb << 5) | (filter << 2) | spi3w_en;
    
    uint8_t data[24] = {0},i=0;
    Debug_Serial.begin(115200);
    Wire.begin();
    I2c_Write_Reg(BMP280_ADDRESS,0xE0,0xB6); //reset
    I2c_Write_Reg(BMP280_ADDRESS,0xF4,ctrl_meas_reg);
    I2c_Write_Reg(BMP280_ADDRESS,0xF5,config_reg);

    Wire.beginTransmission(BMP280_ADDRESS);
    Wire.write(0x88);
    delayMicroseconds(75); //等待 保证数据完整
    Wire.endTransmission(1);
    Wire.requestFrom(BMP280_ADDRESS,22);
    while(Wire.available()){
        data[i] = Wire.read();
        i++;
    }
    ALL_SENSORS::dig_T1 = (data[1] << 8) | data[0];
    ALL_SENSORS::dig_T2 = (data[3] << 8) | data[2];
    ALL_SENSORS::dig_T3 = (data[5] << 8) | data[4];
    ALL_SENSORS::dig_P1 = (data[7] << 8) | data[6];
    ALL_SENSORS::dig_P2 = (data[9] << 8) | data[8];
    ALL_SENSORS::dig_P3 = (data[11]<< 8) | data[10];
    ALL_SENSORS::dig_P4 = (data[13]<< 8) | data[12];
    ALL_SENSORS::dig_P5 = (data[15]<< 8) | data[14];
    ALL_SENSORS::dig_P6 = (data[17]<< 8) | data[16];
    ALL_SENSORS::dig_P7 = (data[19]<< 8) | data[18];
    ALL_SENSORS::dig_P8 = (data[21]<< 8) | data[20];
    ALL_SENSORS::dig_P9 = (data[23]<< 8) | data[22];
    if (ALL_SENSORS::isTestMode){
        Debug_Serial.println("BMP280 init message:");
        Debug_Serial.print("dig_T1:");
        Debug_Serial.println(ALL_SENSORS::dig_T1);
        Debug_Serial.print("dig_T2:");
        Debug_Serial.println(ALL_SENSORS::dig_T2);
        Debug_Serial.print("dig_T3:");
        Debug_Serial.println(ALL_SENSORS::dig_T3);
        Debug_Serial.print("dig_P1:");
        Debug_Serial.println(ALL_SENSORS::dig_P1);
        Debug_Serial.print("dig_P2:");
        Debug_Serial.println(ALL_SENSORS::dig_P2);
        Debug_Serial.print("dig_P3:");
        Debug_Serial.println(ALL_SENSORS::dig_P3);
        Debug_Serial.print("dig_P4:");
        Debug_Serial.println(ALL_SENSORS::dig_P4);
        Debug_Serial.print("dig_P5:");
        Debug_Serial.println(ALL_SENSORS::dig_P5);
        Debug_Serial.print("dig_P6:");
        Debug_Serial.println(ALL_SENSORS::dig_P6);
        Debug_Serial.print("dig_P7:");
        Debug_Serial.println(ALL_SENSORS::dig_P7);
        Debug_Serial.print("dig_P8:");
        Debug_Serial.println(ALL_SENSORS::dig_P8);
        Debug_Serial.print("dig_P9:");
        Debug_Serial.println(ALL_SENSORS::dig_P9);
    }
    return 1; //初始化成功
}

// 获取BMP280数据
void ALL_SENSORS::GetBMP280Data(int *temp, int *pres)
{
    int i = 0;
    uint32_t data[6] = {0};
    Debug_Serial.print("BMP280[Init]:");
    Debug_Serial.println(Wire.available());
    Wire.beginTransmission(BMP280_ADDRESS);
    Wire.write(0xF7);
    Wire.endTransmission();
    Wire.requestFrom(BMP280_ADDRESS,6);
    Debug_Serial.println(Wire.available());
    while(Wire.available()){
        data[i] = Wire.read();
        i++;
    }
    signed long int presVal = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    signed long int tempVal = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);

    if (isTestMode){
        Debug_Serial.print("BMP280[Raw data]:");
        for (int i = 0; i < 6; i++){
            Debug_Serial.print(data[i], HEX);
            Debug_Serial.print(" ");
        }
        Debug_Serial.println();
        Debug_Serial.print("BMP280[Raw data] temp:");
        Debug_Serial.println(tempVal);
        Debug_Serial.print("BMP280[Raw data] pres:");
        Debug_Serial.println(presVal);
    }
    
    *temp = (int)calibration_T(tempVal)/100.0;
    *pres = (int)calibration_P(presVal)/100.0;
}

signed long int ALL_SENSORS::calibration_T(signed long int adc_T)
{
    
    signed long int var1, var2, T;
    var1 = ((((adc_T >> 3) - ((signed long int)ALL_SENSORS::dig_T1<<1))) * ((signed long int)ALL_SENSORS::dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((signed long int)ALL_SENSORS::dig_T1)) * ((adc_T>>4) - ((signed long int)ALL_SENSORS::dig_T1))) >> 12) * ((signed long int)ALL_SENSORS::dig_T3)) >> 14;
    
    ALL_SENSORS::t_fine = var1 + var2;
    T = (ALL_SENSORS::t_fine * 5 + 128) >> 8;
    return T; 
}

signed long int ALL_SENSORS::calibration_P(signed long int adc_P)
{
    signed long int var1, var2;
    unsigned long int P;
    var1 = (((signed long int)ALL_SENSORS::t_fine)>>1) - (signed long int)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11) * ((signed long int)ALL_SENSORS::dig_P6);
    var2 = var2 + ((var1*((signed long int)ALL_SENSORS::dig_P5))<<1);
    var2 = (var2>>2)+(((signed long int)ALL_SENSORS::dig_P4)<<16);
    var1 = (((ALL_SENSORS::dig_P3 * (((var1>>2)*(var1>>2)) >> 13)) >>3) + ((((signed long int)ALL_SENSORS::dig_P2) * var1)>>1))>>18;
    var1 = ((((32768+var1))*((signed long int)ALL_SENSORS::dig_P1))>>15);
    if (var1 == 0)
    {
        return 0;
    }    
    P = (((unsigned long int)(((signed long int)1048576)-adc_P)-(var2>>12)))*3125;
    if(P<0x80000000)
    {
       P = (P << 1) / ((unsigned long int) var1);   
    }
    else
    {
        P = (P / (unsigned long int)var1) * 2;    
    }
    var1 = (((signed long int)ALL_SENSORS::dig_P9) * ((signed long int)(((P>>3) * (P>>3))>>13)))>>12;
    var2 = (((signed long int)(P>>2)) * ((signed long int)ALL_SENSORS::dig_P8))>>13;
    P = (unsigned long int)((signed long int)P + ((var1 + var2 + ALL_SENSORS::dig_P7) >> 4));
    return P;
}

/********************************************************************************************************************************/
//AHT10代码

//获取AHT10数据
void ALL_SENSORS::GetAHT10Data(float* temp_val, float* humi_val){
    int data[6] = {0};
    Wire.beginTransmission(AHT_ADRESS);
    Wire.write(0xAC); // 温度测量
    Wire.write(0x33);
    //Wire.write(0x00);   [纪念此BUG]
    Wire.endTransmission();
    Wire.requestFrom(AHT_ADRESS, 6);
    while (Wire.available()){
        for (int i = 0; i < 6; i++){
            data[i] = Wire.read();
        }
        if (data[0] && 0x08 == 0){ // 检测校准使能位（为了兼容旧版本的AHT10）
            Wire.beginTransmission(AHT_ADRESS);
            Wire.write(0xE1); // 初始化
            Wire.write(0x08);
            Wire.write(0x00);
            Wire.endTransmission();
            Debug_Serial.println("init AHT10");
        }
        else{
            Wire.beginTransmission(AHT_ADRESS); // 读取数据
            Wire.write(0xAC); // 温度测量
            Wire.write(0x33);
            Wire.write(0x00);
            Wire.endTransmission();
            delayMicroseconds(75);
        }
    }
    int humi_raw = ((data[1] << 12) | (data[2] << 4)) | (data[3] >> 4);
    int temp_raw = ((data[3] & 0x0f) << 16) | (data[4] << 8) | data[5];
    *humi_val = (humi_raw / pow(2, 20)) * 100.0;
    *temp_val = (temp_raw / pow(2, 20)) * 200.0 - 50.0;
}

/********************************************************************************************************************************/
//PMS7003I代码

//获取PMS7003I数据 | 传入一个长度为12的整形数组，用于存储数据 | 返回1表示获取成功，0表示获取失败
// 数组的顺序为：PM1.0(std), PM2.5(std), PM10(std), PM1.0(env), PM2.5(env), PM10(env), 0.3um, 0.5um, 1.0um, 2.5um, 5.0um, 10.0um
uint8_t ALL_SENSORS::GetPMS7003IData(int *data) {
    Wire.beginTransmission(PMS7003I_ADDRESS);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(PMS7003I_ADDRESS, 32);
    if (Wire.available() >= 32) {
        byte buffer[32];
        for (int i = 0; i < 32; i++) {
            buffer[i] = Wire.read();
        }
        for (int i = 0; i < 12; i++){
            data[i] = (buffer[i*2+5] | buffer[i*2+4] << 8);
        }
        /*
        unsigned int checksum = buffer[0] | buffer[1] << 8;
        unsigned int pm10_standard = (buffer[5] | buffer[4] << 8);
        unsigned int pm25_standard = (buffer[7] | buffer[6] << 8);
        unsigned int pm100_standard = (buffer[9] | buffer[8] << 8);
        unsigned int pm10_ambient = (buffer[11] | buffer[10] << 8);
        unsigned int pm25_ambient = (buffer[13] | buffer[12] << 8);
        unsigned int pm100_ambient = (buffer[15] | buffer[14] << 8);
        unsigned int particles_03um = (buffer[17] | buffer[16] << 8);
        unsigned int particles_05um = (buffer[19] | buffer[18] << 8);
        unsigned int particles_10um = (buffer[21] | buffer[20] << 8);
        unsigned int particles_25um = (buffer[23] | buffer[22] << 8);
        unsigned int particles_50um = (buffer[25] | buffer[24] << 8);
        unsigned int particles_100um = (buffer[27] | buffer[26] << 8);

        for (int i = 0; i < 32; i++)
        {
            Debug_Serial.print(buffer[i], HEX);
            Debug_Serial.print(" ");
        }
        
        Debug_Serial.println();
        Debug_Serial.print("Checksum: ");
        Debug_Serial.println(checksum);
        Debug_Serial.print("PM1.0 (Standard): ");
        Debug_Serial.print(pm10_standard);
        Debug_Serial.println(" ug/m3");
        Debug_Serial.print("PM2.5 (Standard): ");
        Debug_Serial.print(pm25_standard);
        Debug_Serial.println(" ug/m3");
        Debug_Serial.print("PM10 (Standard): ");
        Debug_Serial.print(pm100_standard);
        Debug_Serial.println(" ug/m3");

        Debug_Serial.print("PM1.0 (Ambient): ");
        Debug_Serial.print(pm10_ambient);
        Debug_Serial.println(" ug/m3");
        Debug_Serial.print("PM2.5 (Ambient): ");
        Debug_Serial.print(pm25_ambient);
        Debug_Serial.println(" ug/m3");
        Debug_Serial.print("PM10 (Ambient): ");
        Debug_Serial.print(pm100_ambient);
        Debug_Serial.println(" ug/m3");

        Debug_Serial.print("Particles 0.3um: ");
        Debug_Serial.print(particles_03um);
        Debug_Serial.println(" #/0.1L");
        Debug_Serial.print("Particles 0.5um: ");
        Debug_Serial.print(particles_05um);
        Debug_Serial.println(" #/0.1L");
        Debug_Serial.print("Particles 1.0um: ");
        Debug_Serial.print(particles_10um);
        Debug_Serial.println(" #/0.1L");
        Debug_Serial.print("Particles 2.5um: ");
        Debug_Serial.print(particles_25um);
        Debug_Serial.println(" #/0.1L");
        Debug_Serial.print("Particles 5.0um: ");
        Debug_Serial.print(particles_50um);
        Debug_Serial.println(" #/0.1L");
        Debug_Serial.print("Particles 10.0um: ");
        Debug_Serial.print(particles_100um);
        Debug_Serial.println(" #/0.1L");
        */
        return 1;
    } else {
        Debug_Serial.println("No data received");
        return 0;
    }
}

/********************************************************************************************************************************/
//SC8代码
// SC8通过PWM输出脉冲输出数据

void ALL_SENSORS::init_SC8() {
    pinMode(SC8_PWM_PIN, INPUT);
}

//传递CO2浓度值，单位ppm
void ALL_SENSORS::GetSC8Data(float *WidthVal) 
{
    unsigned long LOWvalue = 0;
    unsigned long HIGHvalue = 0;
    unsigned long stateVal = 0;
    while(digitalRead(SC8_PWM_PIN) == LOW); // 等待低电平 
    stateVal = micros(); // 获取并存储当前时间防止卡死
    while (digitalRead(SC8_PWM_PIN) == HIGH) {
        LOWvalue = micros(); // 获取当前时间
        if (LOWvalue - stateVal > 2000000){
            Debug_Serial.println("Error: Pulse width too long!");
            break;
        }
    }
    stateVal = micros(); // 获取并存储当前时间防止卡死
    while (digitalRead(SC8_PWM_PIN) == LOW){
        HIGHvalue = micros(); // 获取当前时间
        if (HIGHvalue - stateVal > 2000000){
            Debug_Serial.println("Error: Pulse width too long!");
        break;
        }
    }
    // Debug_Serial.println(LOWvalue); // 打印低电平时间
    // Debug_Serial.println(HIGHvalue); // 打印高电平时间
    *WidthVal = (float)((1004000 - (HIGHvalue - LOWvalue))-2000)*5/1000; // 计算脉冲宽度
}
/********************************************************************************************************************************/
//WZ-S代码
/*
    WZ_S通过串口通信，数据帧格式为：
    0xFF        起始字符
    CH2O=0x17   气体名称(CH2O)
    Ppb=0x04    单位(Ppb)
    0x00        小数位
    0x00        气体浓度高度位
    0x25        气体浓度低位
    0x07        满量程高位
    0xD0        满量程低位
    0x25        校验位

    气体浓度值=气体浓度高位*256+气体浓度低位（单位：ppb）
*/

//初始化WZ-S传感器
void ALL_SENSORS::init_WZS(int RX_PIN, int TX_PIN) {
    WZS_Serial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
}

//传递CH2O浓度值，单位ppm
uint8_t ALL_SENSORS::GetWZSData(float *data) { 
    int flag_end = false;
    int flag_start = false;
    int count = 0;
    byte buffer[9] = {};
    while (flag_end == false)
    {
        if (WZS_Serial.available() > 0) {//接收到数据
            byte inChar = WZS_Serial.read();
            buffer[count] =inChar;
            if (buffer[count] == 0xFF){//接收到起始标志
                count = 0;  
                flag_start = true;
            }
            count++;
            if (count >=9){//接收9个byte数据
                count = 0;
                if (flag_start){
                    /*
                    for (int i = 0 ; i < 9 ; i++){
                        Debug_Serial.print(buffer[i], HEX);//以16进制输出接收到的数据
                        Debug_Serial.print(",");
                    }
                    Debug_Serial.println("");
                    */
                    if(buffer[8]==FucCheckSum(buffer,9)){ //校验  
                        flag_end=true;
                    }
                }
            }
        }
    }
    // int h1 = (int)buffer[2]*256+(int)buffer[3]; //不同版本使用不同的方法获取数值
    *data = ((int)buffer[4]*256+(int)buffer[5])/(int)buffer[2]/1000.0;
    return 1;
}

unsigned char ALL_SENSORS::FucCheckSum(unsigned char *i, unsigned char ln){
    unsigned char j, tempq=0; i+=1;
    for(j=0; j<(ln-2); j++){
        tempq+=*i;
        i++; 
    }
    tempq=(~tempq)+1;
    return(tempq);
}
/********************************************************************************************************************************/
//CCS811代码

//初始化CCS811传感器
void ALL_SENSORS::init_CCS811() {
    bool ok = ccs811.begin();
    if (!ok) Debug_Serial.println("CCS811 begin failed!");
    ok = ccs811.start(CCS811_MODE_1SEC);
    if (!ok) Debug_Serial.println("CCS811 start failed!");
}

//获取CCS811数据 | CO2浓度值，单位ppm | TVOC浓度值，单位ppb
// 返回1表示获取成功，0表示等待新数据，-1表示获取失败
uint8_t ALL_SENSORS::GetCCS811Data(uint16_t *CO2Val, uint16_t *TVOCVal) {
    uint16_t eco2, etvoc, errstat, raw;
    ccs811.read(&eco2, &etvoc, &errstat, &raw);
    if (errstat==CCS811_ERRSTAT_OK){
        *CO2Val = eco2;
        *TVOCVal = etvoc;
        return 1;
    } else if( errstat==CCS811_ERRSTAT_OK_NODATA ) {
        Debug_Serial.println("CCS811: waiting for (new) data");
        return 0;
    } else if( errstat & CCS811_ERRSTAT_I2CFAIL ) { 
        Debug_Serial.println("CCS811: I2C error");
    } else {
        Debug_Serial.print("CCS811: errstat="); Debug_Serial.print(errstat,HEX); 
        Debug_Serial.print("="); Debug_Serial.println( ccs811.errstat_str(errstat) ); 
    }
    return -1;
}