#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char *ap_ssid = "ESP32_SPIFFS_Manager";
const char *ap_password = "12345678";

AsyncWebServer server(80);

// 嵌入的 HTML 页面作为字符串常量
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>SPIFFS File Manager</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background-color: #f7f7f7;
        }
        .container {
            width: 80%;
            margin: 0 auto;
            padding: 20px;
            background-color: #fff;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            border-radius: 8px;
            margin-top: 50px;
        }
        h1 {
            text-align: center;
            color: #333;
        }
        form {
            display: flex;
            justify-content: center;
            margin-bottom: 20px;
        }
        input[type="file"] {
            margin-right: 10px;
        }
        input[type="submit"] {
            padding: 5px 10px;
            background-color: #007BFF;
            color: #fff;
            border: none;
            border-radius: 4px;
            cursor: pointer;
        }
        input[type="submit"]:hover {
            background-color: #0056b3;
        }
        a {
            color: #007BFF;
            text-decoration: none;
        }
        a:hover {
            text-decoration: underline;
        }
        ul {
            list-style-type: none;
            padding: 0;
        }
        li {
            margin: 10px 0;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>SPIFFS File Manager</h1>
        <form method="POST" action="/upload" enctype="multipart/form-data">
            <input type="file" name="file">
            <input type="submit" value="Upload">
        </form>
        <br>
        <a href="/list">List Files</a>
    </div>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);

  // 初始化 SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // 设置 AP 模式
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // 处理文件上传
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "File Uploaded");
  }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      Serial.printf("UploadStart: %s\n", filename.c_str());
      if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
      }
      File file = SPIFFS.open("/" + filename, FILE_WRITE);
      if (!file) {
        Serial.println("Failed to open file for writing");
        return;
      }
      file.write(data, len);
      file.close();
    } else {
      File file = SPIFFS.open("/" + filename, FILE_APPEND);
      if (!file) {
        Serial.println("Failed to open file for writing");
        return;
      }
      file.write(data, len);
      file.close();
    }
    if (final) {
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
    }
  });

  // 处理文件列表请求
  server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request) {
    String output = "<h1>File List</h1>";
    output += "<ul>";
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
      output += "<li>" + String(file.name()) + " - <a href=\"/download?file=" + String(file.name()) + "\">Download</a> - <a href=\"/delete?file=" + String(file.name()) + "\">Delete</a></li>";
      file = root.openNextFile();
    }
    output += "</ul>";
    request->send(200, "text/html", output);
  });

  // 处理文件下载请求
  server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("file")) {
      String filename = request->getParam("file")->value();
      File file = SPIFFS.open("/" + filename, FILE_READ);
      if (file) {
        request->send(file, filename, "application/octet-stream");
        file.close();
      } else {
        request->send(404, "text/plain", "File Not Found");
      }
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // 处理文件删除请求
  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("file")) {
      String filename = request->getParam("file")->value();
      if (SPIFFS.remove("/" + filename)) {
        request->send(200, "text/plain", "File Deleted");
      } else {
        request->send(404, "text/plain", "File Not Found");
      }
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // 处理根请求
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // 启动服务器
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
}