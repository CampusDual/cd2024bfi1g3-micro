#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include "SparkFun_SHTC3.h"

SHTC3 mySHTC3;  // Instancia de SHTC3 class

String ssid = "";
String password = "";

const char* apSSID = "ESP32_AP";
const char* apPassword = "12345678";
WiFiServer apServer(80); // Servidor en modo AP

const char* serverName = "http://192.168.1.128:8000";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

Preferences datosPersistentes;

float degC;
float hr;

void setup() {
  Serial.begin(115200);

  // Inicializar memoria persistente
  datosPersistentes.begin("datos", false);

  // Leer credenciales guardadas
  ssid = leer("ssid");
  password = leer("password");

  Serial.println("Intentando conectar al WiFi guardado...");
  WiFi.begin(ssid.c_str(), password.c_str());

  // Intentar conectarse durante 10 segundos
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado al WiFi.");
    Serial.print("IP local: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nNo se pudo conectar al WiFi. Iniciando en modo AP...");
    createAP(); // Inicia el AP si falla la conexión WiFi
  }

  // Configurar sensor
  Wire.begin();
  Serial.print("Iniciando sensor. Status: ");
  errorDecoder(mySHTC3.begin());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && (millis() - lastTime) > timerDelay) {
    sendData();
    lastTime = millis();
  } else if (WiFi.getMode() == WIFI_AP) {
    // Modo Access Point: manejar solicitudes de configuración
    handleAPRequests();
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
  datosPersistentes.putString(key, dato);
  datosPersistentes.end();
}

//Se debe pasar la clave a buscar escrita entre ""
String leer(const char* key) {
  //El segundo parametro, es lo que devuelve si no encuentra la key que se le pasa
  return datosPersistentes.getString(key, "No encontrado");
}

void sendData() {
  WiFiClient client;
  HTTPClient http;

  mySHTC3.update();  // Recoger medidas y actualizar valores de Tª y RH
  degC = mySHTC3.toDegC();
  hr = mySHTC3.toPercent();

  // Data to send with HTTP POST
  String data = "tempC=" + String(degC) + "&hr=" + String(hr);

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // Send HTTP POST request
  int httpResponseCode = http.POST(data);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  // Free resources
  http.end();
}

void createAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPassword);
  apServer.begin();

  Serial.print("AP iniciado. IP del AP: ");
  Serial.println(WiFi.softAPIP());
}

void handleAPRequests() {
  WiFiClient client = apServer.available(); // Esperar clientes en el servidor AP
  if (client) {
    Serial.println("Cliente conectado");
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("POST") != -1) {
      Serial.println("Procesando solicitud POST...");
      String postData = "";

      // Leer datos enviados en el cuerpo de la solicitud POST
      while (client.available()) {
        postData += char(client.read());
      }

      // Asegurarse de que los datos contienen las credenciales
      if (postData.indexOf("ssid=") != -1 && postData.indexOf("password=") != -1) {
        // Extraer SSID y contraseña de los datos POST
        String receivedSSID = postData.substring(postData.indexOf("ssid=") + 5, postData.indexOf("&"));
        String receivedPassword = postData.substring(postData.indexOf("password=") + 9);
        receivedSSID.trim();
        receivedPassword.trim();

        // Guardar las credenciales en memoria persistente
        escribir("ssid", receivedSSID);
        escribir("password", receivedPassword);

        // Imprimir credenciales en la consola
        Serial.println("Credenciales recibidas:");
        Serial.println("SSID: " + receivedSSID);
        Serial.println("Contraseña: " + receivedPassword);

        // Responder al cliente con éxito
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<html><body><h1>Credenciales guardadas con éxito.</h1>");
        client.println("<p>El dispositivo se reiniciará para aplicar los cambios.</p></body></html>");
        client.stop();

        // Reiniciar para aplicar las nuevas credenciales
        ESP.restart();
      } else {
        // Responder con error si los datos son inválidos
        client.println("HTTP/1.1 400 Bad Request");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<html><body><h1>Error: Credenciales no válidas.</h1></body></html>");
        client.stop();
      }
    } else {
      // Manejar solicitudes GET (mostrar formulario)
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html><body><h1>Configuración WiFi</h1>");
      client.println("<form method='POST' action='/'>");
      client.println("SSID:<br><input type='text' name='ssid'><br><br>");
      client.println("Contraseña:<br><input type='text' name='password'><br><br>");
      client.println("<button type='submit'>Guardar</button>");
      client.println("</form></body></html>");
      client.stop();
    }
  }
}
