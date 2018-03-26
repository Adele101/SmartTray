// Libraries to include.
#include "Wire.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "HX711.h"

// Wifi and server credentials
#define wifi_ssid "University of Washington" // WiFi name
#define wifi_password "" // WiFi password
#define mqtt_server "mediatedspaces.net" // Class server.
#define mqtt_user "hcdeiot" // Shared class login info.
#define mqtt_pass "esp8266"
#define topic_name "SmartTray451" // Unique topic name to subscribe to

// Initializes load sensor
HX711 scale(12, 13); //HX711 scale(6, 5);
float calibration_factor = -375;
float units;
float ounces;

// Initializes the wifi
WiFiClient espClient;
PubSubClient client(espClient);
char incoming[100]; //an array to hold the incoming message

// Connects to wifi
void setup_wifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Callback for incoming messages
void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, "SmartTray451") == 0) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    // When the message is arrived, the current load is measured
    units = scale.get_units(),10;
    if (units < 0) {
    units = 0.00;
    }
    if (!(units < 0)) { // This is where the WoZ comes in, the load sensor is tricky to work with
      client.publish("ForgotSomething", "1");
    }
  }
}

// Function to reconnect the ESP8266 if it becomes disconnected.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("AdeleParsons", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(topic_name);
      client.subscribe("SmartTray451");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Setup load sensor
  Serial.println("HX711 weighing");
  scale.set_scale(calibration_factor);
  scale.tare();
  Serial.println("Readings:");

  // Setup wifi and server
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
