#include "SparkFun_SHTC3.h"

SHTC3 mySHTC3;                                           // Instance of sensor class

float degC;                                              // Degrees Celsius
float rh;                                                // Relative Humidity Percentage
String mac = "F0:F5:BD:A9:2C:80";                        // Device MAC Adress

String payload;

void setup() 
{
  Serial.begin(115200);                                  // Begin Serial 
  while(Serial == false){};                              // Wait for serial connection to start up
  Wire.begin();
  Serial.print("Iniciando sensor. Status: ");
  errorDecoder(mySHTC3.begin());
  Serial.println();
}

void loop() 
{
  mySHTC3.update();                                           // Update the values of the sensor
  processMeasurements();                                      // Store values in variables and create payload for server
  Serial.print("Payload: ");
  Serial.println(payload);
  delay(5000);                                                // Delay for the data rate you want - note that measurements take ~10 ms so the fastest data rate is 100 Hz (when no delay is used)
}

void processMeasurements()
{
  if(mySHTC3.lastStatus == SHTC3_Status_Nominal)              // If the sensor is working normally
  {
    degC = mySHTC3.toDegC();
    rh = mySHTC3.toPercent();
    payload = "{\"data\": {\"DEV_MAC\": \"" + mac + "\", \"ME_TEMP\": " + degC + ", \"ME_HUMIDITY\": " + rh + "}}";
  }
  else
  {
    Serial.print("Update failed, error: "); 
    errorDecoder(mySHTC3.lastStatus);
    Serial.println();
  }
}

void errorDecoder(SHTC3_Status_TypeDef message)                             // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
{
  switch(message)
  {
    case SHTC3_Status_Nominal : Serial.print("Nominal"); break;
    case SHTC3_Status_Error : Serial.print("Error"); break;
    case SHTC3_Status_CRC_Fail : Serial.print("CRC Fail"); break;
    default : Serial.print("Unknown return code"); break;
  }
}
