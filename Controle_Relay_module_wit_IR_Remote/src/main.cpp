#include <Arduino.h>
#include <IRremote.hpp>

// Connect the OUT/SIGNAL pin of the IR receiver module to this GPIO.
// Change this value if your receiver is wired to another safe input pin.
constexpr uint8_t IR_RECEIVE_PIN = 27;

// Connect the relay module input pin to this GPIO.
// Change this value to the pin you wired to the relay module.
constexpr uint8_t RELAY_PIN = 26;

// Set to true if the relay module is active LOW (energized when pin is LOW).
constexpr bool RELAY_ACTIVE_LOW = false;

// These codes are the two buttons you captured from the remote.
constexpr uint32_t RELAY_ON_COMMAND = 0x40;
constexpr uint32_t RELAY_OFF_COMMAND = 0x19;

void printDecodedSignal();
void setRelayState(bool on);

void setup()
{
  Serial.begin(115200);
  delay(500);

  pinMode(RELAY_PIN, OUTPUT);
  setRelayState(false); // start with relay off

  Serial.println();
  Serial.println(F("ESP32 infrared receiver"));
  Serial.print(F("IR signal pin: GPIO "));
  Serial.println(IR_RECEIVE_PIN);
  Serial.print(F("Relay control pin: GPIO "));
  Serial.println(RELAY_PIN);
  Serial.println(F("Press a button on the remote..."));

  // Most three-pin IR receiver modules provide an active-low digital output.
  // LED feedback is disabled so no board LED pin is claimed by the library.
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
}

void loop()
{
  if (!IrReceiver.decode())
  {
    return;
  }

  printDecodedSignal();

  const IRData &data = IrReceiver.decodedIRData;
  if (data.command == RELAY_ON_COMMAND)
  {
    Serial.println(F("Relay command: ON"));
    setRelayState(true);
  }
  else if (data.command == RELAY_OFF_COMMAND)
  {
    Serial.println(F("Relay command: OFF"));
    setRelayState(false);
  }
  else
  {
    Serial.println(F("Relay command: unknown code, no action taken."));
  }

  // Re-enable reception after the decoded data has been read and printed.
  IrReceiver.resume();
}

void setRelayState(bool on)
{
  if (RELAY_ACTIVE_LOW)
  {
    digitalWrite(RELAY_PIN, on ? LOW : HIGH);
  }
  else
  {
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  }
}

void printDecodedSignal()
{
  const IRData &data = IrReceiver.decodedIRData;

  if ((data.flags & IRDATA_FLAGS_WAS_OVERFLOW) != 0)
  {
    Serial.println(F("WARNING: IR receive buffer overflow; signal was too long."));
    Serial.println(F("Increase RAW_BUFFER_LENGTH if this happens repeatedly."));
    return;
  }

  Serial.println(F("\n--- IR signal received ---"));
  IrReceiver.printIRResultShort(&Serial);

  Serial.print(F("Protocol: "));
  Serial.println(getProtocolString(data.protocol));
  Serial.print(F("Address:  0x"));
  Serial.println(data.address, HEX);
  Serial.print(F("Command:  0x"));
  Serial.println(data.command, HEX);
  Serial.print(F("Raw data: 0x"));
  Serial.println(static_cast<uint64_t>(data.decodedRawData), HEX);
  Serial.print(F("Repeat:   "));
  Serial.println((data.flags & IRDATA_FLAGS_IS_REPEAT) != 0 ? F("yes") : F("no"));

  // Raw mark/space timings are useful for unknown or unsupported protocols.
  IrReceiver.printIRResultRawFormatted(&Serial, true);
}
