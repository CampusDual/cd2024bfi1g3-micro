#include <Preferences.h>

String wifi = "Wifilocas";
String passWifi = "EstonoesUnapass";
String wifiLeido;
String passWifiLeido;

Preferences preferences;

void setup() {
  Serial.begin(9600);
  delay(2000);
  preferences.begin("datos-guardados", false);
}

void loop() {
  Serial.println("Opciones [Escribir: E | Leer: L | Mostrar: M]");
  if (Serial.available() > 0) {
    String textoSerial = Serial.readStringUntil('\n');

    if (textoSerial.equalsIgnoreCase("E")) {
      Serial.println("Seleccionado escribir.");
      escribir(0, wifi);
      escribir(1, passWifi);
    } else if (textoSerial.equalsIgnoreCase("L")) {
      Serial.println("Seleccionado leer.");
      wifiLeido = leer(0);
      passWifiLeido = leer(1);
    } else if (textoSerial.equalsIgnoreCase("M")) {
      Serial.println("Seleccionado mostrar.");
      mostrar();
    }
  }

  delay(500);
}

void escribir(int posicion, String value) {
  String key = "key" + String(posicion);
  preferences.putString(key.c_str(), value);
}

String leer(int posicion) {
  String key = "key" + String(posicion);
  return preferences.getString(key.c_str(), "");
}

void mostrar() {
  wifiLeido = leer(0);
  Serial.print("SSID wifi: ");
  Serial.println(wifiLeido);
  passWifiLeido = leer(1);
  Serial.print("Contraseña leida: ");
  Serial.println(passWifiLeido);
}

