// MyLibrary/Constants.h
#ifndef CONFIGHTML_H
#define CONFIGHTML_H

namespace ConfigHtml {
    String configHtml = R"rawliteral(<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Input Test</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
    input[type="text"] { padding: 10px; font-size: 16px; width: 80%; }
    button { padding: 10px 20px; font-size: 16px; margin-top: 10px; }
  </style>
</head>
<body>
  <h1>Send Data to ESP32</h1>
  <input type="text" id="inputData" placeholder="Enter your message here">
  <button onclick="sendData()">Send</button>
  <script>
    function sendData() {
      const data = document.getElementById("inputData").value;
      fetch(`/send?data=${encodeURIComponent(data)}`)
        .then(response => response.text())
        .then(text => alert("Response from ESP32: " + text))
        .catch(error => alert("Error: " + error));
    }
  </script>
</body>
</html>)rawliteral";
}

#endif // CONFIGHTML_H
