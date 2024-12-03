#include "SparkFun_SHTC3.h"

SHTC3 mySHTC3;                                                // Instancia de SHTC3 class

void setup() {

  Serial.begin(115200);
  while(Serial == false){};                                   // Esperamos a que el Serial se inicie
  Serial.println("Lecturas de temperatura(ºC) y humedad relativa");
  Wire.begin();                                               // Iniciamos la librería Wire, que permite comunicación vía I2C
  Serial.print("Iniciando sensor. Status: ");
  errorDecoder(mySHTC3.begin());                              // Iniciamos el sensor
  Serial.println();
  Serial.println("\n\n");

  delay(5000);
}

void loop() {
  mySHTC3.update();                                           // Recoger medidas y actualizar valores de Tª y RH
  printInfo();
  delay(1000);
}

void printInfo()
{
  if(mySHTC3.lastStatus == SHTC3_Status_Nominal)              // Comprobamos status del último comando lanzado con mySHTC3
  {
    Serial.print("RH = "); 
    Serial.print(mySHTC3.toPercent());
    Serial.print("%, T = "); 
    Serial.print(mySHTC3.toDegC());
    Serial.println(" Cº");
  }
  else
  {
    Serial.print("Algo ha fallado: "); 
    errorDecoder(mySHTC3.lastStatus);
    Serial.println();
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
