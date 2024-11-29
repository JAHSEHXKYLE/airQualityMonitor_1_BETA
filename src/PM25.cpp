#include <Arduino.h>

int measurePin = 0;            
int ledPower = 11;              
 
unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;
 
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;
 
void setup(){
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);
}
 
void loop(){
  digitalWrite(ledPower,LOW);
  delayMicroseconds(samplingTime);
 
  voMeasured = analogRead(measurePin);
 
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH);
  delayMicroseconds(sleepTime);
 
  calcVoltage = voMeasured*(5.0/1024);
  dustDensity = 0.17*calcVoltage-0.1;
 
  if ( dustDensity < 0)
  {
    dustDensity = 0.00;
  }
 
  Serial.print("Air Quality:");
  Serial.print(voMeasured);
  
 /*
  Serial.print("当前实时电压电压:");
  Serial.println(calcVoltage); */
 
  Serial.print("   PM2.5:");
  Serial.print(dustDensity);
  Serial.println("%");
 
  delay(1000);
}
