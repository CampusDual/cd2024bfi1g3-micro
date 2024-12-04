#include <WiFi.h>
#include <PubSubClient.h>

#define SSID "Wifi_SSID"
#define PSW "WIfi_PSW"

#define MQTT_IP "broker.mqtt.cool"
#define MQTT_User ""
#define MQTT_Pw ""
#define MQTT_Client "TestClient"
#define MQTT_Port 1883
#define MQTT_Topic "/test_micro"

WiFiClient espClient;
PubSubClient client(espClient);

String client_id = MQTT_Client;
//client_id = String(WiFi.macAddress());

void setup()
{
  Serial.begin(115200);
  WiFi.begin(SSID,PSW);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Conectando a Wifi...");
  }

  Serial.print("Conectado a Wifi ");
  Serial.println(SSID);

    //Conectarse a MQTT
  client.setServer(MQTT_IP, MQTT_Port);
  client.setCallback(callback);

  while (!client.connected()) {
      
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), MQTT_User, MQTT_Pw)) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  // publish and subscribe
  //client.publish(MQTT_Topic, "Hi EMQX I'm ESP32 ^^");
  //client.subscribe(MQTT_Topic);
}

void loop() {
  client.loop();

  if(WiFi.status() == WL_CONNECTED){
    if(client.connected()){
      client.publish(MQTT_Topic, "Mensaje test");
    }else{
      reconnect_MQTT();
    }

  } else {
    reconnect_WiFi();
  }
  
  delay(1000);
}

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 Serial.println();
 Serial.println("-----------------------");
}

void reconnect_MQTT() {
  // Loop until we're reconnected
  while (!client.connected()) {
      
      //Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), MQTT_User, MQTT_Pw)) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
}

void reconnect_WiFi() {

  WiFi.begin(SSID,PSW);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Conectando a Wifi...");
  }

  Serial.print("Conectado a Wifi ");
  Serial.println(SSID);


}
