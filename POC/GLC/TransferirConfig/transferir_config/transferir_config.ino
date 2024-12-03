/*
  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1
*/

#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>

#endif

// Credenciales de la red del microcontrolador.
const char *ssid = "yourAp";
const char *password = "hola1234";

NetworkServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // A valid password must have more than 7 characters
  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation failed.");
    while (1);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
}

void loop() {
  NetworkClient client = server.accept();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    String ssidReceived = "";
    String passReceived = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("<br><br>");
            client.print("<form method='GET' action='/'>");
            client.print("<label for='ssid'>WiFi SSID: </label>");
            client.print("<input type='text' id='ssid' name='ssid'><br>");
            client.print("<label for='pass'>Password: </label>");
            client.print("<input type='password' id='pass' name='pass'><br>");
            client.print("<input type='submit' value='Enviar'>");
            client.print("</form>");

            client.println();
            
            break;
          } else {
            if (currentLine.startsWith("GET /?ssid=")) {
              int ssidStart = currentLine.indexOf("ssid=") + 5;
              int ssidEnd = currentLine.indexOf("&pass=");
              int passStart = currentLine.indexOf("pass=") + 5;
              int passEnd = currentLine.indexOf(" ", passStart);

              ssidReceived = currentLine.substring(ssidStart, ssidEnd);
              passReceived = currentLine.substring(passStart, passEnd);

              ssidReceived.replace("+", " ");
              passReceived.replace("+", " ");

              Serial.print("SSID received: ");
              Serial.println(ssidReceived);
              Serial.print("Password received: ");
              Serial.println(passReceived);
            }

            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
