#include <EEPROM.h>
 
String wifi = "WifiPrueba";
String passWifi = "ThisPassword";
String wifiLeido;
String passWifiLeido;
 
int direccionSSID = 0;        // Dirección donde guardar el SSID
int direccionPass = 100;      // Dirección donde guardar la contraseña (evita solapamiento)
 
void setup() {
  Serial.begin(9600);
  delay(2000);
  EEPROM.begin(512);
  Serial.println("Escribe 'G' para guardar, 'L' para leer, 'M' para mostrar datos.");
}
 
void loop() {
  if (Serial.available() > 0) {
    String textoSerial = Serial.readStringUntil('\n');
    textoSerial.trim();
    if (textoSerial.equalsIgnoreCase("G")) {
      escribir(wifi, direccionSSID);
      escribir(passWifi, direccionPass);
      Serial.println("Credenciales guardadas.");
    } else if (textoSerial.equalsIgnoreCase("L")) {
      wifiLeido = leer(direccionSSID);
      passWifiLeido = leer(direccionPass);
      Serial.println("Credenciales leídas.");
    } else if (textoSerial.equalsIgnoreCase("M")) {
      mostrar();
    } else {
      Serial.println("Comando no reconocido. Usa 'E', 'L' o 'M'.");
    }
  }
}
 
void escribir(String frase, int posicion){
  byte longitud = frase.length();
  EEPROM.put(posicion, longitud);
  posicion += sizeof(byte);
 
  for(int i = 0; i < longitud; i++){
    EEPROM.put( posicion + i, frase.charAt(i));
  }
  EEPROM.commit();
}
 
String leer(int posicion){
  byte longitud;
  EEPROM.get(posicion, longitud);
  posicion += sizeof(byte);
 
  String resultado = "";
  for (int i = 0; i < longitud; i++) {
    char c;
    EEPROM.get(posicion + i, c);
    resultado += c;
  }
  return resultado;
}
 
void mostrar(){
  Serial.print("SSID wifi: ");
  Serial.println(wifiLeido);
  Serial.print("Contraseña leida: ");
  Serial.println(passWifiLeido);
}
