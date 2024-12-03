#include "EspMQTTClient.h"
#include <WiFi.h>

#define SSID "Wifi.DC"
#define PSW "22334455"

EspMQTTClient client(
  "Wifi ssid",
  "wifi pw",
  "broker.mqtt.cool",  // MQTT Broker server ip
  "MQTTUsername",   // Can be omitted if not needed
  "MQTTPassword",   // Can be omitted if not needed
  "TestClient",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

void setup()
{
  Serial.begin(115200);
  WiFi.begin(SSID,PSW);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Conencatado a Wifi...");
  }

  Serial.print("Conectado a Wifi ");
  Serial.println(SSID);

  if(!client.isMqttConnected()){
    client.loop();
  }

  // Optional functionalities of EspMQTTClient
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  client.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
}

void loop() {
  while(WiFi.status() == WL_CONNECTED){
    while(client.isConnected()){
      Serial.println("Escribir mensaje");
      if (Serial.available() > 0) {
        String textoSerial = Serial.readStringUntil('\n');

        client.publish("/test_micro", textoSerial);
      }

    }//fin while mqtt

  }//fin while wifi

  if(WiFi.status() != WL_CONNECTED){
    WiFi.begin(SSID,PSW);
  }
  if(!client.isMqttConnected()){
    client.loop();
  }

  delay(500);
}
