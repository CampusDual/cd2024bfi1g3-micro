#include <EEPROM.h>

String wifi = "Wifilocas";
String passWifi = "EstonoesUnapass";
String wifiLeido;
String passWifiLeido;
int direccion;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(2000);
  EEPROM.begin(512);
}

void loop() {
  if (Serial.available() > 0) {
    String textoSerial = Serial.readStringUntil('\n');

    if (textoSerial.equalsIgnoreCase("E")) {
      escribir(wifi,0);
      escribir(passWifi,1);
    } else if (textoSerial.equalsIgnoreCase("L")) {
      //leer();
    } else if (textoSerial.equalsIgnoreCase("M")) {
      mostrar();
    }
  }

  delay(2000);
}

void escribir(String frase, int posicion){
  byte num;
  for(int i=0;i<=posicion;i++){
    if(i=0){
      direccion=0;
    }else{
      EEPROM.get(direccion,num);
      direccion+=num+1;
    }
  }
  EEPROM.put( direccion, frase.length()); 
  direccion++;
  for(int i=0;i<frase.length();i++){
    EEPROM.put( direccion, frase.charAt(i)); 
    direccion++;
  }
  EEPROM.commit();
  
}

String leer(int posicion){
  byte num;
  String resultado="";
  for(int i=0;i<=posicion;i++){
    if(i=0){
      direccion=0;
    }else{
      EEPROM.get(direccion,num);
      direccion+=num+1;
    }
  }



 return resultado;
}

void mostrar(){
  wifiLeido = leer(0);
  Serial.print("SSID wifi: ");
  Serial.println(wifiLeido);
  passWifiLeido = leer(1);
  Serial.print("Contraseña leida: ");
  Serial.println(passWifiLeido);
}