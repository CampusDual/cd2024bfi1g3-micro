#include <WiFi.h>

#define SSID "..."
#define PSW "...."

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(SSID,PSW);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Conencatado a Wifi...");
  }

  Serial.println("Conectado a Wifi");
}

void loop() {
  // put your main code here, to run repeatedly:
  while(WiFi.status() == WL_CONNECTED){
    delay(500);
    Serial.println("Wifi conectado");
  }
}
