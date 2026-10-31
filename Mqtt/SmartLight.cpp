
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *WIFI_SSID = "Astraval";
const char *WIFI_PASSWORD = "astraval25";

// MQTT
const char *MQTT_HOST = "iotroot.astraval.com";
const int MQTT_PORT = 1883;
const char *MQTT_USER = "retrofit1";
const char *MQTT_PASSWORD = "password";
const char *MQTT_CLIENT_ID =
    "8bae179f-a612-4276-9760-ee423bd042bb";

const char *MQTT_TOPIC = "/iot/4/data/command";

// Relay
#define RELAY_PIN 26
#define RELAY_ON LOW
#define RELAY_OFF HIGH

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

bool relayState = false;
unsigned long lastReconnectAttempt = 0;

// Publish response to SAME topic
void publishResponse(bool state)
{
  const char *response = state
                             ? "{\"status\":\"on\"}"
                             : "{\"status\":\"off\"}";

  if (mqtt.publish(MQTT_TOPIC, response))
  {
    Serial.print("Response: ");
    Serial.println(response);
  }
  else
  {
    Serial.println("Publish failed");
  }
}

// Receive MQTT commands
void mqttCallback(char *topic, byte *payload,
                  unsigned int length)
{

  String message = "";

  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  message.trim();

  Serial.print("Received: ");
  Serial.println(message);

  // Remove whitespace for simple matching
  message.replace(" ", "");
  message.replace("\n", "");
  message.replace("\r", "");

  bool requestedState;

  if (message == "{\"status\":\"on\"}")
  {
    requestedState = true;
  }
  else if (message == "{\"status\":\"off\"}")
  {
    requestedState = false;
  }
  else
  {
    Serial.println("Invalid command");
    return;
  }

  // Prevent processing own response
  if (requestedState == relayState)
  {
    return;
  }

  // Control relay
  digitalWrite(
      RELAY_PIN,
      requestedState ? RELAY_ON : RELAY_OFF);

  delay(50);

  // Verify output pin state
  if (digitalRead(RELAY_PIN) ==
      (requestedState ? RELAY_ON : RELAY_OFF))
  {

    relayState = requestedState;

    Serial.println(
        relayState ? "Relay ON" : "Relay OFF");

    publishResponse(relayState);
  }
}

// Connect MQTT
void connectMQTT()
{

  if (mqtt.connected())
    return;

  Serial.println("Connecting MQTT...");

  if (mqtt.connect(
          MQTT_CLIENT_ID,
          MQTT_USER,
          MQTT_PASSWORD))
  {

    Serial.println("MQTT Connected");

    mqtt.subscribe(MQTT_TOPIC, 1);

    Serial.println("Subscribed");
  }
  else
  {
    Serial.print("MQTT Error: ");
    Serial.println(mqtt.state());
  }
}

void setup()
{

  Serial.begin(115200);

  digitalWrite(RELAY_PIN, RELAY_OFF);
  pinMode(RELAY_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(mqttCallback);

  connectMQTT();
}

void loop()
{

  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.reconnect();
    delay(100);
    return;
  }

  if (!mqtt.connected())
  {
    if (millis() - lastReconnectAttempt >= 5000)
    {
      lastReconnectAttempt = millis();
      connectMQTT();
    }
  }
  else
  {
    mqtt.loop();
  }
}
