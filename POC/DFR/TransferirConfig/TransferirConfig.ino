#include <WiFi.h>

const char *ssid = "patata";
const char *password = "123";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando modo WiFi...");
  WiFi.softAP(ssid, password);
  Serial.print("Dirección IP del AP: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Nuevo cliente conectado");
    String request = client.readStringUntil('\r');
    Serial.println("Petición recibida:");
    Serial.println(request);

    if (request.indexOf("POST") != -1) {
      while (client.available()) {
        String postData = client.readStringUntil('\r');
        if (postData.length() > 0) {
          if (postData.indexOf("ssid=") != -1 && postData.indexOf("password=") != -1) {
            String receivedSSID = postData.substring(postData.indexOf("ssid=") + 5, postData.indexOf("&"));
            String receivedPassword = postData.substring(postData.indexOf("password=") + 9);
            receivedSSID.replace("+", " ");
            receivedPassword.replace("+", " ");
            Serial.println("SSID recibida:");
            Serial.println(receivedSSID);
            Serial.println("Contraseña recibida:");
            Serial.println(receivedPassword);
          }
        }
      }
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("<head></head>");
    client.println("<body>");
    client.println("<h1>Configurar SSID y Contraseña</h1>");
    client.println("<form method=\"POST\" action=\"/\">");
    client.println("SSID:<br>");
    client.println("<input name=\"ssid\"><br><br>");
    client.println("Contraseña:<br>");
    client.println("<input name=\"password\"><br><br>");
    client.println("<button>Enviar</button>");
    client.println("</form>");
    client.println("</body>");
    client.println("</html>");
    client.stop();
  }
}
