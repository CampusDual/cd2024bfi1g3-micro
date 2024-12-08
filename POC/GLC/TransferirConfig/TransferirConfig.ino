#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h> // Librería ESPAsyncWebServer

// Credenciales del AP
const char *ssid = "yourAp";
const char *password = "hola1234";

// Inicializar servidor
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Configuración del AP
  WiFi.softAP(ssid, password);
  Serial.print("Dirección IP del AP: ");
  Serial.println(WiFi.softAPIP());

  // Ruta GET para mostrar el formulario
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
      </head>
      <body>
        <h1>Servidor ESP32</h1>
        <form method="POST" action="/">
          <label for="ssid">WiFi SSID:</label>
          <input type="text" id="ssid" name="ssid" required><br>
          <label for="pass">Password:</label>
          <input type="password" id="pass" name="pass" required><br>
          <input type="submit" value="Enviar">
        </form>
      </body>
      </html>
    )rawliteral");
  });

  // Ruta POST para recibir las credenciales
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    String ssidReceived = "";
    String passReceived = "";

    // Obtener parámetros del cuerpo de la solicitud
    if (request->hasParam("ssid", true) && request->hasParam("pass", true)) {
      ssidReceived = request->getParam("ssid", true)->value();
      passReceived = request->getParam("pass", true)->value();

      Serial.println("SSID recibido: " + ssidReceived);
      Serial.println("Password recibido: " + passReceived);

      // Respuesta de confirmación
      request->send(200, "text/html", "<h1>Datos recibidos!</h1><p>SSID: " + ssidReceived + "</p>");
    } else {
      // Respuesta en caso de error
      request->send(400, "text/html", "<h1>Error: Faltan datos</h1>");
    }
  });

  // Iniciar el servidor
  server.begin();
}