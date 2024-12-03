#include "SparkFun_SHTC3.h"

SHTC3 mySHTC3;

void setup() {
  Serial.begin(115200);
  while (Serial == false);
  Serial.println("SHTC3 Example 1 - Basic Readings");
  Wire.begin();
  Serial.print("Beginning sensor. Result = ");
  errorDecoder(mySHTC3.begin());
  Serial.println();
  Serial.println("\n\n");
  Serial.println("Waiting for 5 seconds so you can read this info ^^^");
  delay(5000);
}

void loop() {
  SHTC3_Status_TypeDef result = mySHTC3.update();
  printInfo();
  delay(190);
}

void printInfo() {
  if (mySHTC3.lastStatus == SHTC3_Status_Nominal) {
    Serial.print("RH = ");
    Serial.print(mySHTC3.toPercent());
    Serial.print("%, T = ");
    Serial.print(mySHTC3.toDegF());
    Serial.println(" deg F");
  } else {
    Serial.print("Update failed, error: ");
    errorDecoder(mySHTC3.lastStatus);
    Serial.println();
  }
}

void errorDecoder(SHTC3_Status_TypeDef message) {
  switch (message) {
    case SHTC3_Status_Nominal: Serial.print("Nominal"); break;
    case SHTC3_Status_Error: Serial.print("Error"); break;
    case SHTC3_Status_CRC_Fail: Serial.print("CRC Fail"); break;
    default: Serial.print("Unknown return code"); break;
  }
}
