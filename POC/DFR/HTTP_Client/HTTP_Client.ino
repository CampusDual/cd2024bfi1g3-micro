#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* network_ssid = "David";
const char* network_password = "abc1234.";
const String server_url = "https://webhook.site/1d24a720-8162-4e47-86a0-2ffe97919f21";

void setup() {
  Serial.begin(115200);
  WiFi.begin(network_ssid, network_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando al Wi-Fi...");
  }
  Serial.println("Conexión Wi-Fi establecida.");
  Serial.print("Dirección IP asignada: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  String data_to_send = "{\"message\": \"hola\"}";
  transmitDataToServer(data_to_send);
  delay(10000);
}

void transmitDataToServer(String data_to_send) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient httpRequest;
    httpRequest.begin(server_url);
    httpRequest.addHeader("Content-Type", "application/json");
    int response_code = httpRequest.POST(data_to_send);

    if (response_code > 0) {
      Serial.print("Datos enviados correctamente. Código de respuesta: ");
      Serial.println(response_code);
      String server_response = httpRequest.getString();
      Serial.println("Respuesta del servidor:");
      Serial.println(server_response);
    } else {
      Serial.print("Error al enviar datos. Código de error: ");
      Serial.println(httpRequest.errorToString(response_code).c_str());
    }
    httpRequest.end();
  } else {
    Serial.println("No hay conexión Wi-Fi. No se pudieron enviar los datos.");
  }
}
