#include <Preferences.h>

Preferences preferences;

const char* ssid = "MiWifi";
const char* password = "MiPassword";

void setup() {
  Serial.begin(115200);
  Serial.println();

  preferences.begin("credentials", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);

  Serial.println("Credenciales guardadas!");

  preferences.end();

  Serial.println("Escribe 'M' para mostrar las credenciales.");
}

void loop() {
  if (Serial.available() > 0) {
    String textoSerial = Serial.readStringUntil('\n');
    textoSerial.trim();
    if (textoSerial.equalsIgnoreCase("M")) {
      mostrar();
    } else {
      Serial.println("Comando no reconocido. Usa 'M'.");
    }
  }
}

void mostrar(){
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Contraseña: ");
  Serial.println(password);
}
