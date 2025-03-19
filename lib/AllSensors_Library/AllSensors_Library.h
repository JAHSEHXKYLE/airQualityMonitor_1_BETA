#ifndef ALL_SENSORS_LIBRARY_H
#define ALL_SENSORS_LIBRARY_H

#include <Arduino.h>
#include <Wire.h>
#include "ccs811.h"

#define BMP280_ADDRESS 0x76
#define AHT_ADRESS 0x38
#define PMS7003I_ADDRESS 0x12

#define SC8_PWM_PIN 42
#define SDA_PIN 2
#define SCL_PIN 1
#define Seri1_RX_PIN 10
#define Seri1_TX_PIN 9
#define UV_PIN 18

#define Debug_Serial Serial
#define WZS_Serial Serial1

extern CCS811 ccs811;

class ALL_SENSORS {
    private:
        static signed long int t_fine;
        static uint16_t dig_T1;
        static int16_t dig_T2;
        static int16_t dig_T3;
        static uint16_t dig_P1;
        static int16_t dig_P2;
        static int16_t dig_P3;
        static int16_t dig_P4;
        static int16_t dig_P5;
        static int16_t dig_P6;
        static int16_t dig_P7;
        static int16_t dig_P8;
        static int16_t dig_P9;
        
    public:
        static bool isTestMode;
        void setTestMode(bool mode);
        void init_all_sensors();
        void I2c_Write_Reg(uint8_t I2c_address, uint8_t reg_address, uint8_t data);
        uint8_t init_BMP280();
        void GetBMP280Data(int *temp, int *pres);
        signed long int calibration_T(signed long int adc_T);
        signed long int calibration_P(signed long int adc_P);
        void GetAHT10Data(float* temp_val, float* humi_val);
        uint8_t GetPMS7003IData(int *data);
        void init_SC8();
        void GetSC8Data(float *WidthVal);
        void init_WZS(int RX_PIN, int TX_PIN);
        uint8_t GetWZSData(float *data);
        void GetUVData(long *data);
        unsigned char FucCheckSum(unsigned char *i, unsigned char ln);
        void init_CCS811();
        uint8_t GetCCS811Data(uint16_t *CO2Val, uint16_t *TVOCVal);
        uint8_t GetAGS10Data(int *data);
};
#endif // ALL_SENSORS_LIBRARY_H