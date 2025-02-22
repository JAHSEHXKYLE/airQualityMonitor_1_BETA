#ifndef ALL_SENSORS_LIBRARY_H
#define ALL_SENSORS_LIBRARY_H

#include <Arduino.h>
#include <Wire.h>
#include "ccs811.h"

#define BMP280_ADDRESS 0x76
#define AHT_ADRESS 0x38
#define PMS7003I_ADDRESS 0x12
#define SC8_PWM_PIN 4
#define Debug_Serial Serial


class AllSensors{
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
        uint8_t init();
        void I2c_Write_Reg(uint8_t I2c_address, uint8_t reg_address, uint8_t data);
        uint8_t init_BMP280();
        void GetBMP280Data(int *temp, int *pres);
        signed long int calibration_T(signed long int adc_T);
        signed long int calibration_P(signed long int adc_P);
        void GetAHT10Data(float* temp_val, float* humi_val);
        void GetPMS7003IData(float* pm1_0, float* pm2_5, float* pm10_0);
};

#endif // ALL_SENSORS_LIBRARY_H