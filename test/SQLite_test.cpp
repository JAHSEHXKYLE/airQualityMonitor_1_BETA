#include <sqlite3.h>
#include <SPIFFS.h>

sqlite3 *db;

void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
 
  int rc = sqlite3_open("/test.db", &db);
  if (rc != SQLITE_OK) {
    Serial.printf("Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  }
  Serial.println("Opened database successfully");
}

void loop() {
  // put your main code here, to run repeatedly:
}