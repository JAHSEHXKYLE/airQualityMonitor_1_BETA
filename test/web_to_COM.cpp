#include <WiFi.h>
#include <WebServer.h>

// WiFi 配置
const char* ssid = "YANG_PC";      // 替换为您的 WiFi 名称
const char* password = "K2003@0315#";  // 替换为您的 WiFi 密码

// 创建 Web 服务器
WebServer server(80);

void handleRoot() {
    // 返回 HTML 页面
    server.send(200, "text/html", R"rawliteral(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>ESP32 输入测试</title>
            <script>
                // 拦截表单提交，使用 Fetch 发送请求
                function submitForm(event) {
                    event.preventDefault(); // 阻止默认提交行为
                    const input = document.getElementById('contentInput').value;
                    
                    // 使用 Fetch API 提交数据
                    fetch('/submit', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                        body: 'content=' + encodeURIComponent(input)
                    })
                    .then(response => response.text())
                    .then(data => {
                        // 显示服务器返回的消息
                        document.getElementById('responseMessage').textContent = data;
                    })
                    .catch(error => {
                        console.error('Error:', error);
                    });
                }
            </script>
        </head>
        <body>
            <h1>输入内容并提交</h1>
            <form onsubmit="submitForm(event)">
                <input type="text" id="contentInput" maxlength="100" placeholder="输入内容">
                <button type="submit">提交</button>
            </form>
            <p id="responseMessage" style="color: green;"></p>
        </body>
        </html>
    )rawliteral");
}

void handleSubmit() {
    // 提取输入内容
    if (server.hasArg("content")) {
        String content = server.arg("content");
        Serial.println("收到输入内容: " + content); // 串口打印内容
        server.send(200, "text/plain", "内容已收到: " + content);
    } else {
        server.send(400, "text/plain", "未找到输入内容");
    }
}

void setup() {
    // 初始化串口
    Serial.begin(115200);
    delay(1000);

    // 连接到 WiFi
    Serial.println("正在连接 WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi 已连接!");
    Serial.println("IP 地址: ");
    Serial.println(WiFi.localIP());

    // 配置 Web 服务器路由
    server.on("/", handleRoot);
    server.on("/submit", handleSubmit);

    // 启动 Web 服务器
    server.begin();
    Serial.println("Web 服务器已启动");
}

void loop() {
    // 处理客户端请求
    server.handleClient();
}
