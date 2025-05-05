#include <AllSensors_Library.h>

ALL_SENSORS allsensors;


void setup() {
    allsensors.setTestMode(true);
    Wire.begin(SDA_PIN, SCL_PIN);
    allsensors.init_all_sensors();
}

void loop() {
    int temperature, pressure;
    float humidity = 0, temperature_aht10 = 0;
    allsensors.GetAHT10Data(&temperature_aht10, &humidity);
    allsensors.GetBMP280Data(&temperature, &pressure);
    
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("C, Pressure: ");
    Serial.print(pressure);
    Serial.print("hPa");
    Serial.print(", Humidity: ");
    Serial.print(humidity);
    Serial.print("%, Temperature AHT10: ");
    Serial.print(temperature_aht10);
    Serial.println("C");
    delay(1000);
}