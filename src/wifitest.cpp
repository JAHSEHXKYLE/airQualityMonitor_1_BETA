#include <WiFi.h>
#include <Arduino.h>

const int led1Pin = 12;
const int led2Pin = 13;
bool led1Flag = false;
bool led2Flag = false;
String receline = "";
String receAll = "";
String led1status = "OFF";
String led2status = "OFF";
const char* ssid = "ESP32wifiTest";
const char* password = "12345678";

WiFiServer server(80);
WiFiClient client;

void pagedisplay();

void setup() {
  Serial.begin(115200);
  Serial.println("Starting WiFi");
  WiFi.softAP(ssid, password);
  server.begin();
  Serial.println("WiFi started");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
}

void loop() {
    client = server.available();
    if (client) {
        Serial.println("New clientc connected to server IP address: " + client.remoteIP().toString());
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.print(c);
                receAll += c;
                if (c == '\n') {
                    if (receline.length() == 0){
                        // getCommand();
                        pagedisplay();
                        break;
                    }
                    else{
                        receline = "";
                    }
                }
                else if (c!= '\r') {
                    receline += c;
                }
            }
        }
        client.stop();
        Serial.println("Client disconnected");
    }
    //ledControl();
    receAll = "";
}

// void ledControl() {
//     digitalWrite(led1Pin, led1Flag);
// }

void pagedisplay(){
    String response = "";
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Connection: close\r\n\r\n";
        response += "<html><head><title>ESP32 WiFi Test</title></head><body>";
        response += "<h1>ESP32 WiFi Test</h1>";
        response += "<form method='POST' action='/'><label for='led1'>LED1:</label><input type='checkbox' id='led1' name='led1' value='ON' ";
        if (led1status == "ON") {
            response += "checked";
        } 
        response += "><br><label for='led2'>LED2:</label><input type='checkbox' id='led2' name='led2' value='ON' ";
        if (led2status == "ON") {
            response += "checked";
        }
        response += "><br><input type='submit' value='Submit'></form>";
        response += "<a href='/led1_on'><button>ON</button></a>";
        response += "</body></html>";
    client.print(response);
    receline = "";
}