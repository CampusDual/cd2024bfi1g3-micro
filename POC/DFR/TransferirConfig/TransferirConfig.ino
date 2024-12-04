#include <WiFi.h>

const char *ssid = "patata";
const char *password = "123";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    if (request.indexOf("POST") != -1) {
      while (client.available()) {
        String postData = client.readStringUntil('\r');
        if (postData.indexOf("ssid=") != -1 && postData.indexOf("password=") != -1) {
          String receivedSSID = postData.substring(postData.indexOf("ssid=") + 5, postData.indexOf("&"));
          String receivedPassword = postData.substring(postData.indexOf("password=") + 9);
          receivedSSID.replace("+", " ");
          receivedPassword.replace("+", " ");
          Serial.println(receivedSSID);
          Serial.println(receivedPassword);
        }
      }
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("<body>");
    client.println("<form method=\"POST\" action=\"/\">");
    client.println("SSID:<br><input name=\"ssid\"><br><br>");
    client.println("Contraseña:<br><input name=\"password\"><br><br>");
    client.println("<button>Enviar</button>");
    client.println("</form>");
    client.println("</body>");
    client.println("</html>");
    client.stop();
  }
}
