#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SSID";
const char* password = "SSID_PASSWORD";
const char* mqtt_server = "iotroot.astraval.com";
const int mqtt_port = 1883;
const char* mqtt_user = "USERNAME";
const char* mqtt_password = "PASSWORD";
const char* mqtt_topic = "TOPIC";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    // Using your exact working Client ID from terminal
    if (client.connect("11fe7bc7-470a-4d04-a3af-16984392ea98", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  
  // Using millis() instead of analogRead(0) to prevent Wi-Fi ADC conflict
  randomSeed(millis());
  delay(2000);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temperature = random(200, 350) / 10.0; // 20.0°C - 35.0°C
  String payload = String("{\"temperature\":") + temperature + "}";

  Serial.print("Publishing: ");
  Serial.println(payload);

  client.publish(mqtt_topic, payload.c_str());
  delay(2000); // send every 2 seconds
}
