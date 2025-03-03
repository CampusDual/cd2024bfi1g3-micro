#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include "SparkFun_SHTC3.h"

AsyncWebServer server(80);
DNSServer dnsServer;
Preferences preferences;

const char* ap_ssid = "ESP32_Config";
const char* ap_password = "12345678";

SHTC3 mySHTC3;
float humidity = 0;
float temperatureNow = 0;
unsigned long lastSendTime = 0;
String server_url = "";

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
          justify-content: center;
          align-items: center;
          height: 100vh;
          background-color: #f4f4f4;
        }
        .container {
          background-color: #fff;
          padding: 20px;
          border-radius: 8px;
          box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
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
        input[type="submit"].reset {
          background-color: #dc3545;
          margin-top: 10px;
        }
      </style>
</head>
<body>
  <div class="container">
    <h1>Configuración</h1>
    <form action="/save" method="POST">
      <label for="ssid">SSID:</label>
      <input type="text" id="ssid" name="ssid" required>
      <label for="password">Contraseña:</label>
      <input type="password" id="password" name="password" required>
      <label for="server_url">URL del servidor:</label>
      <input type="text" id="server_url" name="server_url" required>
      <input type="submit" value="Guardar" class="save">
    </form>
    <form action="/reset" method="POST">
      <input type="submit" value="Resetear Configuración" class="reset">
    </form>
  </div>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_AP_STA);
  preferences.begin("wifi-config", false);
  bool configured = preferences.getBool("configured", false);
  String savedSSID = preferences.getString("ssid", "");
  String savedPass = preferences.getString("password", "");
  server_url = preferences.getString("server_url", "");
  startCaptivePortal();
  if (configured && !savedSSID.isEmpty() && !savedPass.isEmpty() && !server_url.isEmpty()) {
    connectToWiFi(savedSSID, savedPass);
  }
  Wire.begin();
  if (mySHTC3.begin() != SHTC3_Status_Nominal) {
    Serial.println("Error al inicializar el sensor SHTC3!");
    while(1);
  }
}

void loop() {
  dnsServer.processNextRequest();
  if (WiFi.status() == WL_CONNECTED) {
    if (millis() - lastSendTime >= 60000) {
      readSensorData();
      sendDataToServer();
      lastSendTime = millis();
    }
  }
}

void startCaptivePortal() {
  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("Access Point iniciado: ");
  Serial.println(WiFi.softAPIP());
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
    String ssid = request->arg("ssid");
    String password = request->arg("password");
    String serverUrl = request->arg("server_url");
    if(ssid.length() > 0 && password.length() > 0 && serverUrl.length() > 0) {
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
      preferences.putString("server_url", serverUrl);
      preferences.putBool("configured", true);
      server_url = serverUrl;
      request->send(200, "text/html", "<script>setTimeout(function(){window.location.href='/';},5000);</script>Configuración guardada. Reconectando...");
      connectToWiFi(ssid, password);
    } else {
      request->send(400, "text/plain", "Todos los campos son requeridos");
    }
  });
  server.on("/reset", HTTP_POST, [](AsyncWebServerRequest *request){
    preferences.clear();
    request->send(200, "text/html", "<script>setTimeout(function(){window.location.href='/';},3000);</script>Configuración reseteada. Reiniciando...");
    delay(3000);
    ESP.restart();
  });
  server.onNotFound([](AsyncWebServerRequest *request){
    request->redirect("http://" + WiFi.softAPIP().toString() + "/");
  });
  server.begin();
}

void connectToWiFi(String ssid, String password) {
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.println("Conectando a WiFi...");
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    if (!MDNS.begin("esp32")) {
      Serial.println("Error iniciando mDNS!");
    }
  } else {
    Serial.println("\nError de conexión! Manteniendo el AP activo para reconfiguración.");
  }
}

void readSensorData() {
  if (mySHTC3.update() == SHTC3_Status_Nominal) {
    temperatureNow = mySHTC3.toDegC();
    humidity = mySHTC3.toPercent();
    Serial.printf("Temperatura: %.2f°C, Humedad: %.2f%%\n", temperatureNow, humidity);
  } else {
    Serial.println("Error leyendo sensor!");
  }
}

void sendDataToServer() {
  if (server_url.isEmpty()) return;
  HTTPClient http;
  String full_url = server_url + "/measurements/measurements";
  http.begin(full_url);
  http.addHeader("Authorization", "Basic YWRtaW5NaWNyb3M6YWRtaW5taWNyb3MxMjM="); //adminmicros 
  http.addHeader("Content-Type", "application/json");
  String payload = "{";
  payload += "\"data\": {";
  payload += "\"DEV_MAC\":\"" + WiFi.macAddress() + "\",";
  payload += "\"ME_TEMP\":" + String(temperatureNow) + ",";
  payload += "\"ME_HUMIDITY\":" + String(humidity);
  payload += "}}";
  int httpCode = http.POST(payload);
  if (httpCode > 0) {
    Serial.println("URL a la que se está enviando: " + full_url);
    Serial.printf("Datos enviados. Código: %d\n", httpCode);
  } else {
    Serial.printf("Error en envío: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}