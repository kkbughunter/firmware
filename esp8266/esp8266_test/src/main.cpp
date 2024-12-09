#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nSetup Started");

  Serial.println("Hello ESP");

  Serial.println("Setup Ended \n");
}

void loop() {

  Serial.println("in loop");
  delay(10000);

}
