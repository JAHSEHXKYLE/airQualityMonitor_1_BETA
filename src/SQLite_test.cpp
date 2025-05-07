#include <sqlite3.h>
#include <SPIFFS.h>

void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  sqlite3 *db;
  int rc = sqlite3_open("/SQLitest/test.db", &db);
}