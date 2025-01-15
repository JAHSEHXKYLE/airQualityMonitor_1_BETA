#include <SPIFFS.h>

void listFiles() {
    File root = SPIFFS.open("/");
    if (!root) {
        Serial.println("Failed to open SPIFFS root");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        Serial.print("File: ");
        Serial.print(file.name());
        Serial.print(", Size: ");
        Serial.println(file.size());
        file = root.openNextFile();
    }
}

void setup() {
    Serial.begin(115200);

    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    listFiles();
}

void loop() {
}
