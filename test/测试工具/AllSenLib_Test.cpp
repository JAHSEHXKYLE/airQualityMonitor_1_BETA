#include <AllSensors_Library.h>

#define BUZZER_PIN 47

ALL_SENSORS all_sensors;

void GetAllData(char array[][10]);

void setup() {
    // int tones[] = {2100, 5210};
    // pinMode(BUZZER_PIN, OUTPUT);
    // tone(BUZZER_PIN, tones[1], 110);
    // delay(210);
    // tone(BUZZER_PIN, tones[0], 210);
    all_sensors.init_all_sensors();
}

void loop() {
    
    char array[19][10];
    GetAllData(array);  
    delay(500);
}

void GetAllData(char array[][10]) {
    float temp_val, humi_val;
    int BMP_temp, BMP_pres;
    int pms_datas[12];
    float co2_val;
    float ch2o_val;
    uint16_t eco2_val, tvoc_val;
    unsigned long get_data_time = micros();
    Debug_Serial.println("Get All Data Ready");
    all_sensors.GetAHT10Data(&temp_val, &humi_val);
    Debug_Serial.printf("AHT10 Data: Temp = %.2f, Humi = %.2f\n", temp_val, humi_val);

    all_sensors.GetBMP280Data(&BMP_temp, &BMP_pres);
    Debug_Serial.printf("BMP280 Data: Temp = %d, Pres = %d\n", BMP_temp, BMP_pres);

    all_sensors.GetPMS7003IData(pms_datas);
    Debug_Serial.printf("PMS7003I Data: PM1.0 = %d, PM2.5 = %d, PM10 = %d\n", pms_datas[0], pms_datas[1], pms_datas[2]);

    all_sensors.GetCCS811Data(&eco2_val, &tvoc_val);
    Debug_Serial.printf("CCS811 Data: eCO2 = %d, TVOC = %d\n", eco2_val, tvoc_val);

    //all_sensors.GetWZSData(&ch2o_val);
    //Debug_Serial.printf("WZS Data: CH2O = %.3f\n", ch2o_val);

    get_data_time = micros() - get_data_time;
    Debug_Serial.printf("\nGet Data Time : %d us", get_data_time);

    //all_sensors.GetSC8Data(&co2_val);
    //Debug_Serial.printf("SC8 Data: CO2 = %.2f\n", co2_val);

    // sprintf(array[0], "%.2f", temp_val);
    // sprintf(array[1], "%.2f", humi_val);
    // sprintf(array[2], "%d", BMP_pres);
    // sprintf(array[3], "%d", pms_datas[3]);   //大气环境下PM1.0 ug/m3
    // sprintf(array[4], "%d", pms_datas[4]);   //大气环境下PM2.5 ug/m3
    // sprintf(array[5], "%d", pms_datas[5]);   //大气环境下PM10 ug/m3
    // sprintf(array[6], "%.2f", co2_val);
    // sprintf(array[7], "%.3f", ch2o_val);
    // sprintf(array[8], "%d", eco2_val);
    // sprintf(array[9], "%d", tvoc_val);

    // sprintf(array[10], "%d", pms_datas[0]);  //PM1.0 ug/m3
    // sprintf(array[11], "%d", pms_datas[1]);  //PM2.5 ug/m3
    // sprintf(array[12], "%d", pms_datas[2]);  //PM10 ug/m3
    // sprintf(array[13], "%d", pms_datas[6]);  //0.1L空气中直径大于0.3μm的颗粒物个数
    // sprintf(array[14], "%d", pms_datas[7]);  //0.1L空气中直径大于0.5μm的颗粒物个数
    // sprintf(array[15], "%d", pms_datas[8]);  //0.1L空气中直径大于1.0μm的颗粒物个数
    // sprintf(array[16], "%d", pms_datas[9]);  //0.1L空气中直径大于2.5μm的颗粒物个数
    // sprintf(array[17], "%d", pms_datas[10]); //0.1L空气中直径大于5.0μm的颗粒物个数
    // sprintf(array[18], "%d", pms_datas[11]); //0.1L空气中直径大于10.0μm的颗粒物个数

    // String str[] = {"温度", "湿度", "气压", "PM1.0", "PM2.5", "PM10", "CO2", "CH2O", "eCO2", "TVOC", "PM1.0", "PM2.5", "PM10", "0.3μm颗粒物", "0.5μm颗粒物", "1.0μm颗粒物", "2.5μm颗粒物", "5.0μm颗粒物", "10.0μm颗粒物"};

    // for (int i = 0; i < 19; i++) {
    //     if (i == 10) Debug_Serial.printf("\n\nOther Data:\n");
    //     Debug_Serial.print(str[i]);
    //     Debug_Serial.print(" : ");
    //     Debug_Serial.println(array[i]);
    // }
    
}