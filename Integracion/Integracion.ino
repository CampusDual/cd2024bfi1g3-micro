#include <WiFi.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Preferences.h>
#include "SparkFun_SHTC3.h"

SHTC3 mySHTC3;  // Instancia de SHTC3 class

// Credenciales de WiFi
String ssid = "";
String password = "";
String mac = "";
String ipServer = "";

// Credenciales del AP
const char* apSSID = "ESP32_AP";
const char* apPassword = "12345678";

// Servidor y almacenamiento persistente
AsyncWebServer server(80);
Preferences datosPersistentes;

// Datos del servidor remoto
String serverName = "http://0.0.0.0:8000";
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

float degC;
float hr;

void setup() {
  Serial.begin(115200);
  datosPersistentes.begin("datos", false);

  // Leer credenciales guardadas
  ssid = leer("ssid");
  password = leer("password");
  ipServer = leer("ipServer");
  Serial.println("SSID guardado: " + ssid);
  Serial.println("Contraseña guardada: " + password);
  Serial.println("MAC guardada: " + mac);
  Serial.println("IP Servidor remoto guardado: " + ipServer);

  //Modificar serverName
  serverName = "http://" + ipServer + ":8000";

  if (ssid != "" && password != "" && ipServer != "") {
    conectarWiFi();
  } else {
    iniciarModoAP();
  }

  readMacAddress(); // Guardar la MAC si no está ya en la EEPROM

  // Configurar sensor
  Wire.begin();
  Serial.print("Iniciando sensor. Status: ");
  errorDecoder(mySHTC3.begin());
  Serial.println();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && (millis() - lastTime) > timerDelay) {
    WiFiClient client;
    HTTPClient http;

    mySHTC3.update();
    degC = mySHTC3.toDegC();
    hr = mySHTC3.toPercent();

    String httpRequestData = "tempC=" + String(degC) + "&hr=" + String(hr) + "&mac=" + String(mac);
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    http.end();

    lastTime = millis();
  } else if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado. Intentando reconectar...");
    conectarWiFi();
  }
}

void errorDecoder(SHTC3_Status_TypeDef message)  // Imprime los status de SHTC3 de forma legible
{
  switch (message) {
    case SHTC3_Status_Nominal: Serial.print("Nominal"); break;
    case SHTC3_Status_Error: Serial.print("Error"); break;
    case SHTC3_Status_CRC_Fail: Serial.print("CRC Fail"); break;
    default: Serial.print("Código de status desconocido"); break;
  }
}

//Se debe pasar la clave escrita entre "" y el dato a guardar en String.
void escribir(const char* key, String dato) {
  datosPersistentes.begin("datos", false);
  datosPersistentes.putString(key, dato);
  datosPersistentes.end();
}

//Se debe pasar la clave a buscar escrita entre ""
String leer(const char* key) {
  //El segundo parametro, es lo que devuelve si no encuentra la key que se le pasa
  return datosPersistentes.getString(key, "");
}

void conectarWiFi() {
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Conectando al WiFi: ");
  Serial.println(ssid);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado a WiFi!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nNo se pudo conectar al WiFi. Iniciando modo AP.");
    iniciarModoAP();
  }
}

void iniciarModoAP() {
  WiFi.softAP(apSSID, apPassword);
  Serial.print("Dirección IP del AP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head>
        <meta name="viewport" content="width=device-width, initial-scale=1" charset=“utf-8”>
      </head>
      <body>
        <h1>Configuración WiFi</h1>
        <form method="POST" action="/">
          <label for="ssid">WiFi SSID:</label>
          <input type="text" id="ssid" name="ssid" required><br>
          <label for="pass">Password:</label>
          <input type="password" id="pass" name="pass" required><br>
          <label for="ip_remote">IP Server remote:</label>
          <input type="text" id="ip_remote" name="ip_remote" required><br>
          <input type="submit" value="Guardar">
        </form>
      </body>
      </html>
    )rawliteral");
  });

  server.on("/", HTTP_POST, [](AsyncWebServerRequest* request) {
    if (request->hasParam("ssid", true) && request->hasParam("pass", true)) {
      ssid = request->getParam("ssid", true)->value();
      password = request->getParam("pass", true)->value();
      ipServer = request->getParam("ip_remote", true)->value();

      Serial.println("Credenciales recibidas:");
      Serial.println("SSID: " + ssid);
      Serial.println("Password: " + password);
      Serial.println("IP Servidor remoto: " + ipServer);

      escribir("ssid", ssid);
      escribir("password", password);
      escribir("ipServer", ipServer);
      datosPersistentes.end();

      request->send(200, "text/html", "<h1>Credenciales guardadas. Reiniciando...</h1>");
      delay(1000);
      ESP.restart();
    } else {
      request->send(400, "text/html", "<h1>Error: Faltan datos</h1>");
    }
  });

  server.begin();
}

void readMacAddress(){
  // Verifica si ya hay una MAC guardada
  mac = leer("mac");
  if (mac != "") {
    Serial.println("MAC ya guardada en EEPROM: " + mac);
    return; // Si ya está guardada, no hacemos nada
  }

  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac); // Obtén la MAC del ESP32 en modo estación
  if (ret == ESP_OK) {
    // Convierte la MAC a un formato estándar
    String macAddress = String(baseMac[0], HEX) + ":" +
                        String(baseMac[1], HEX) + ":" +
                        String(baseMac[2], HEX) + ":" +
                        String(baseMac[3], HEX) + ":" +
                        String(baseMac[4], HEX) + ":" +
                        String(baseMac[5], HEX);

    macAddress.toUpperCase(); // Convertir a mayúsculas para un formato estándar
    Serial.println("Dirección MAC leída: " + macAddress);

    // Guarda la MAC en la EEPROM
    escribir("mac", macAddress);
    Serial.println("MAC guardada en EEPROM.");
    mac = macAddress; // Actualiza la variable global para otros usos
  } else {
    Serial.println("Error al leer la dirección MAC");
  }
}