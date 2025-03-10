#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include "SparkFun_SHTC3.h"
#include <Base64.h>

AsyncWebServer server(80);
DNSServer dnsServer;
Preferences preferences;

const char* ap_ssid = "ESP32_Config";
const char* ap_password = "12345678";
long reset_time=0;
SHTC3 mySHTC3;
float humidity = 0;
float temperatureNow = 0;
unsigned long lastSendTime = 0;
String server_url = "";
String authCredentialsBase64 = ""; 
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
      <label for="password">Contraseña WiFi:</label>
      <input type="password" id="password" name="password" required>
      <label for="server_url">URL del servidor:</label>
      <input type="text" id="server_url" name="server_url" placeholder="http://IPServidor:Puerto" required>
      <label for="auth_user">Usuario:</label>
      <input type="text" id="auth_user" name="auth_user" required>
      <label for="auth_password">Contraseña de autenticación:</label>
      <input type="password" id="auth_password" name="auth_password" required>
      <input type="submit" value="Guardar" class="save">
    </form>
    <form action="/reset" method="POST">
      <input type="submit" value="Resetear Configuración" class="reset">
    </form>
  </div>
</body>
</html>
)rawliteral";
void startCaptivePortal();
void connectToWiFi(String ssid, String password);
void readSensorData();
void sendDataToServer();


void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_AP_STA);
  preferences.begin("wifi-config", false);
  bool configured = preferences.getBool("configured", false);
  String savedSSID = preferences.getString("ssid", "");
  String savedPass = preferences.getString("password", "");
  server_url = preferences.getString("server_url", "");
  authCredentialsBase64 = preferences.getString("auth_base64", "");
  startCaptivePortal();
  if (configured && !savedSSID.isEmpty() && !savedPass.isEmpty() && !server_url.isEmpty() && !authCredentialsBase64.isEmpty()) {
    connectToWiFi(savedSSID, savedPass);
  }
  Wire.begin();
  if (mySHTC3.begin() != SHTC3_Status_Nominal) {
    Serial.println("Error initializing the SHTC3 sensor!");
    while(1);
  }
}

void loop() {
  if(reset_time!=0 && millis()>reset_time){
     WiFi.softAPdisconnect(true);   
    dnsServer.stop();              
    preferences.clear();
    preferences.end();
    ESP.restart();
  }
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
  Serial.print("Access Point started: ");
  Serial.println(WiFi.softAPIP());
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
    String ssid = request->arg("ssid");
    String password = request->arg("password");
    String serverUrl = request->arg("server_url");
    String auth_user = request->arg("auth_user");           
    String auth_password = request->arg("auth_password");
    if(ssid.length() > 0 && password.length() > 0 && serverUrl.length() > 0 &&
       auth_user.length() > 0 && auth_password.length() > 0) {
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
      preferences.putString("server_url", serverUrl);
      String credentials = auth_user + ":" + auth_password;
      authCredentialsBase64 = base64::encode(reinterpret_cast<const uint8_t*>(credentials.c_str()), credentials.length());
      preferences.putString("auth_base64", authCredentialsBase64);
      preferences.putBool("configured", true);
      server_url = serverUrl;
      request->send(200, "text/html", R"rawliteral(
        <!DOCTYPE html>
        <html>
        <head>
          <meta charset="UTF-8">
          <meta name="viewport" content="width=device-width, initial-scale=1.0">
          <title>Configuración Guardada</title>
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
              text-align: center;
            }
            h1 {
              color: #333;
              font-size: 20px;
            }
          </style>
        </head>
        <body>
          <div class="container">
            <h1>Configuración guardada. Reconectando...</h1>
          </div>
          <script>
            setTimeout(function(){ window.location.href='/'; }, 5000);
          </script>
        </body>
        </html>
        )rawliteral");

      connectToWiFi(ssid, password);
    } else {
      request->send(400, "text/plain", "Todos los campos son obligatorios");
    }
  });
  server.on("/reset", HTTP_POST, [](AsyncWebServerRequest *request){
    
    // preferences.end();
    reset_time=millis()+5000;
    request->send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Reset Completo</title>
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
              text-align: center;
            }
            h1 {
              color: #333;
              font-size: 20px;
            }
          </style>
      </head>
      <body>
        <div class="container">
        <h1>Configuración reseteada. Reiniciando...</h1>
        </div>
      </body>
      </html>
    )rawliteral");

});

  server.onNotFound([](AsyncWebServerRequest *request){
    request->redirect("http://" + WiFi.softAPIP().toString() + "/");
  });
  server.begin();
}

void connectToWiFi(String ssid, String password) {
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.println("Connecting to WiFi...");
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    if (!MDNS.begin("esp32")) {
      Serial.println("Error starting mDNS!");
    }
  } else {
    Serial.println("\nConnection failed! Keeping AP active for reconfiguration.");
  }
}

void readSensorData() {
  if (mySHTC3.update() == SHTC3_Status_Nominal) {
    temperatureNow = mySHTC3.toDegC();
    humidity = mySHTC3.toPercent();
    Serial.printf("Temperature: %.2f°C, Humidity: %.2f%%\n", temperatureNow, humidity);
  } else {
    Serial.println("Error reading the sensor!");
  }
}

void sendDataToServer() {
  if (server_url.isEmpty()) return;
  HTTPClient http;
  String full_url = server_url + "/measurements/measurements";
  http.begin(full_url);
  http.setTimeout(20000);
  http.addHeader("Authorization", "Basic " + authCredentialsBase64);
  http.addHeader("Content-Type", "application/json");
  String payload = "{";
  payload += "\"data\": {";
  payload += "\"DEV_MAC\":\"" + WiFi.macAddress() + "\","; 
  payload += "\"ME_TEMP\":" + String(temperatureNow) + ",";
  payload += "\"ME_HUMIDITY\":" + String(humidity);
  payload += "}}";
  Serial.println("JSON: "+payload);
  Serial.println("AuthCredentials: "+ authCredentialsBase64);
  Serial.println("Full url: "+full_url);
  int httpCode = http.POST(payload);
  if (httpCode > 0) {
    Serial.println("URL being sent to: " + full_url);
    Serial.printf("Data sent. Code: %d\n", httpCode);
  } else {
    Serial.printf("Error sending data: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}