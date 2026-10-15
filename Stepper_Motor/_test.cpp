#include <Arduino.h>

#define PUL 25
#define DIR 26
#define ENA 27
#define IR 32

const int stepsPerRevolution = 6400;
bool objectDetected = false;

void rotateMotor(bool direction)
{
  digitalWrite(DIR, direction);
  delayMicroseconds(20);

  for (int i = 0; i < stepsPerRevolution; i++)
  {
    digitalWrite(PUL, HIGH);
    delayMicroseconds(500);
    digitalWrite(PUL, LOW);
    delayMicroseconds(500);
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IR, INPUT);

  digitalWrite(ENA, HIGH); // Verify enable polarity
}

void loop()
{
  bool detected = digitalRead(IR) == LOW;

  if (detected && !objectDetected)
  {
    Serial.println("Object Detected!");
    rotateMotor(LOW);
    objectDetected = true;
  }

  if (!detected && objectDetected)
  {
    Serial.println("Object Removed!");
    rotateMotor(HIGH);
    objectDetected = false;
  }

  delay(50);
}
