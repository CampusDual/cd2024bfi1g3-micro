#include <Wire.h>
#include <SparkFun_SHTC3.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>

SHTC3 mySHTC3;

String ssid = "";
String password = "";
String serverUrl = "";
String deviceName = "123";
String devicePassword = "123";  

const char* apSSID = "ESP32-Config";
const char* apPassword = "123456789";

AsyncWebServer server(80);
DNSServer dnsServer;
Preferences preferences;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Configuración</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 0;
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
      height: 100vh;
      background-color: #f4f4f4;
    }
    .container {
      background-color: #fff;
      padding: 20px;
      border-radius: 8px;
      box-shadow: 0 0 10px rgba(0,0,0,0.1);
      max-width: 400px;
      width: 100%;
    }
    h1 {
      text-align: center;
      color: #333;
    }
    form {
      display: flex;
      flex-direction: column;
    }
    label {
      margin-bottom: 5px;
      color: #555;
    }
    input[type="text"],
    input[type="password"] {
      padding: 10px;
      margin-bottom: 15px;
      border: 1px solid #ccc;
      border-radius: 4px;
    }
    input[type="submit"] {
      padding: 10px;
      border: none;
      border-radius: 4px;
      color: #fff;
      cursor: pointer;
    }
    input[type="submit"].save {
      background-color: #007bff;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Configuración</h1>
    <form action="/setWiFi" method="POST">
      <label for="ssid">SSID:</label>
      <input type="text" id="ssid" name="ssid" required>
      <label for="password">Contraseña:</label>
      <input type="password" id="password" name="password" required>
      <label for="serverUrl">URL del servidor:</label>
      <input type="text" id="serverUrl" name="serverUrl" required>
      <input type="submit" value="Guardar" class="save">
    </form>
  </div>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  preferences.begin("wifiCreds", false);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  serverUrl = preferences.getString("serverUrl", ""); 
  ssid = "";
  password = "";
  if (ssid != "" && password != "") {
    Serial.println("Intentando conectar a Wi-Fi...");
    WiFi.begin(ssid.c_str(), password.c_str());
    WiFiConecction();
  } else {
    WiFi.softAP(apSSID, apPassword);
    Serial.println("Configurando Access Point...");
    Serial.print("IP del AP: ");
    Serial.println(WiFi.softAPIP());
    startCaptivePortal();
  }
  Wire.begin();
  if (mySHTC3.begin() != 0) {
    Serial.println("Error al inicializar el sensor SHTC3.");
    while (1);
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    SHTC3_Status_TypeDef result = mySHTC3.update();   
    float temp = mySHTC3.toDegC();
    float humidity = mySHTC3.toPercent();
    String uniqueID = WiFi.macAddress();
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.print(" °C, Humedad: ");
    Serial.print(humidity);
    Serial.println(" %");
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Authorization", "Basic YWRtaW5NaWNyb3M6YWRtaW5taWNyb3MxMjM=");
    http.addHeader("Content-Type", "application/json");
    String payload = "{";
    payload += "\"data\": {";
    payload += "\"DEV_MAC\":\"" + WiFi.macAddress() + "\",";
    payload += "\"ME_TEMP\":" + String(temp) + ",";
    payload += "\"ME_HUMIDITY\":" + String(humidity);
    payload += "}}";
    int httpCode = http.POST(payload);
    if (httpCode > 0) {
        Serial.printf("Código de respuesta del servidor: %d\n", httpCode);
        String response = http.getString(); 
    } else {
        Serial.printf("Error en la conexión: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  delay(20000);  
}

void startCaptivePortal() {
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });
  server.on("/setWiFi", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("password", true) && request->hasParam("serverUrl", true)) {
      ssid = request->getParam("ssid", true)->value();
      password = request->getParam("password", true)->value();
      serverUrl = request->getParam("serverUrl", true)->value();
      DynamicJsonDocument doc(1024);
      doc["ssid"] = ssid;
      doc["password"] = password;
      doc["serverUrl"] = serverUrl;
      String jsonString;
      serializeJson(doc, jsonString);
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
      preferences.putString("serverUrl", serverUrl);
      Serial.println("Credenciales y URL del servidor guardadas en la memoria NVS.");
      Serial.print("Configuración en JSON: ");
      Serial.println(jsonString);
      request->send(200, "text/html", "<html><body><h2>Configuracion guardada Reiniciando...</h2></body></html>");
      WiFi.begin(ssid.c_str(), password.c_str());
      WiFiConecction();
    } else {
      request->send(400, "text/html", "<html><body><h2>Error: Faltan parametros.</h2></body></html>");
    }
  });
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });
  server.begin();
}

void WiFiConecction() {
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(1000);
    Serial.println("Conectando a Wi-Fi...");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conexión Wi-Fi exitosa!");
    Serial.print("IP del dispositivo: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("No se pudo conectar a Wi-Fi. Iniciando AP...");
    WiFi.softAP(apSSID, apPassword);
    Serial.print("IP del AP: ");
    Serial.println(WiFi.softAPIP());
    startCaptivePortal();
  }
}
