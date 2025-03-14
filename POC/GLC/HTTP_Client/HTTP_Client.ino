#include <WiFi.h>
#include <HTTPClient.h>
#include "SparkFun_SHTC3.h"

SHTC3 mySHTC3;                                                // Instancia de SHTC3 class

const char* ssid = "MIWIFI_RkKN";
const char* password = "G5dazapS";

const char* serverName = "http://192.168.1.128:8000";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

float degC;
float hr;

void setup() {
  Serial.begin(115200);

  while(Serial == false){};                                   // Esperamos a que el Serial se inicie
  Serial.println("Lecturas de temperatura(ºC) y humedad relativa");
  Wire.begin();                                               // Iniciamos la librería Wire, que permite comunicación vía I2C
  Serial.print("Iniciando sensor. Status: ");
  errorDecoder(mySHTC3.begin());                              // Iniciamos el sensor
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");


      mySHTC3.update();                                       // Recoger medidas y actualizar valores de Tª y RH

      degC = mySHTC3.toDegC();
      hr = mySHTC3.toPercent();

      // Data to send with HTTP POST
      String httpRequestData = "tempC=" + String(degC) + "&hr=" + String(hr);           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void errorDecoder(SHTC3_Status_TypeDef message)               // Imprime los status de SHTC3 de forma legible
{
  switch(message)
  {
    case SHTC3_Status_Nominal : Serial.print("Nominal"); break;
    case SHTC3_Status_Error : Serial.print("Error"); break;
    case SHTC3_Status_CRC_Fail : Serial.print("CRC Fail"); break;
    default : Serial.print("Código de status desconocido"); break;
  }
}