#include <Arduino.h>
#include <MQTT.h>
#include <WiFi.h>

// Set these two values for your Wi-Fi network.
constexpr char WIFI_SSID[] = "ssid-name";
constexpr char WIFI_PASSWORD[] = "ssid-password";

constexpr char MQTT_HOST[] = "iotroot.astraval.com";
constexpr uint16_t MQTT_PORT = 1883;
constexpr char MQTT_USERNAME[] = "username";
constexpr char MQTT_PASSWORD[] = "passwd";
constexpr char MQTT_CLIENT_ID[] = "clientid";
constexpr char MQTT_TOPIC[] = "/iot/store/4/watertank";

constexpr uint8_t FLOW_SENSOR_PIN = 27;
constexpr uint32_t MEASUREMENT_INTERVAL_MS = 2000;

// YF-S201: frequency (Hz) = 7.5 * flow rate (L/min), nominally
// 450 pulses per litre. Adjust this after calibration if required.
constexpr float PULSES_PER_LITRE = 450.0F;

WiFiClient networkClient;
MQTTClient mqttClient(256);

volatile uint32_t pulseCount = 0;
uint32_t lastMeasurementMs = 0;
uint32_t lastReconnectAttemptMs = 0;
double totalLitres = 0.0;

void IRAM_ATTR onFlowPulse() {
  pulseCount++;
}

void connectWiFi() {
  Serial.printf("Connecting to Wi-Fi %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }

  Serial.printf(" connected; IP address: %s\n",
                WiFi.localIP().toString().c_str());
}

void connectMqtt() {
  if (mqttClient.connected() || WiFi.status() != WL_CONNECTED) {
    return;
  }

  Serial.print("Connecting to MQTT broker...");
  if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.println(" connected");
  } else {
    Serial.printf(" failed (error %d)\n", mqttClient.lastError());
  }
}

void publishMeasurement(uint32_t now) {
  const uint32_t elapsedMs = now - lastMeasurementMs;
  lastMeasurementMs = now;

  noInterrupts();
  const uint32_t pulses = pulseCount;
  pulseCount = 0;
  interrupts();

  const double litres = pulses / static_cast<double>(PULSES_PER_LITRE);
  const double flowLitresPerMinute = litres * 60000.0 / elapsedMs;
  totalLitres += litres;

  char payload[80];
  snprintf(payload, sizeof(payload),
           "{\"value\":%.3f,\"total\":%.3f}",
           flowLitresPerMinute, totalLitres);

  Serial.printf("Flow: %.3f L/min, total: %.3f L, pulses: %lu\n",
                flowLitresPerMinute, totalLitres,
                static_cast<unsigned long>(pulses));

  // Do not send zero-flow measurements. Publish only when the sensor has
  // produced at least one pulse during this two-second measurement window.
  if (pulses == 0) {
    return;
  }

  if (!mqttClient.connected()) {
    Serial.println("MQTT offline; measurement not published");
    return;
  }

  // retained=false, QoS=1 (equivalent to mosquitto_pub -q 1)
  if (mqttClient.publish(MQTT_TOPIC, payload, false, 1)) {
    Serial.printf("Published to %s: %s\n", MQTT_TOPIC, payload);
  } else {
    Serial.println("MQTT publish failed");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), onFlowPulse, FALLING);

  connectWiFi();
  mqttClient.begin(MQTT_HOST, MQTT_PORT, networkClient);
  connectMqtt();
  lastMeasurementMs = millis();
}

void loop() {
  mqttClient.loop();

  const uint32_t now = millis();
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
  } else if (!mqttClient.connected() &&
             now - lastReconnectAttemptMs >= 5000) {
    lastReconnectAttemptMs = now;
    connectMqtt();
  }

  if (now - lastMeasurementMs >= MEASUREMENT_INTERVAL_MS) {
    publishMeasurement(now);
  }

  delay(10);
}
