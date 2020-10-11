
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "functions.h"

char* ssid = "Dialog 4G wish";
char* password = "wishez1234";
const char* mqtt_server = "192.168.1.2";

//char* ssid = "Eng-Student";
//char* password = "3nG5tuDt";
//const char* mqtt_server = "10.30.15.0";

#define current_road 1
char* esp_clinet = "ESP8266Client_1";

byte willQoS = 0;
const char* willTopic = "willTopic";
const char* willMessage = "offline";
boolean willRetain = true;


WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  //while (!client.connected()) {
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (client.connect(esp_clinet, willTopic, willQoS, willRetain, willMessage)) {
    Serial.println("connected");
    client.publish(willTopic, "online");
    // Subscribe
    //client.subscribe("esp32/output");
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 1 seconds");
    // Wait 1 seconds before retrying
    delay(1000);
  }
  //}
}


void setup() {
  // put your setup code here, to run once:
  client.setKeepAlive(10);
  pin_setup();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  //  if (!client.connected()) {
  //    reconnect();
  //  }

}

int prev_lane_0 = vehicle_count[0];
int prev_lane_1 = vehicle_count[2];
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  get_count();

  if (vehicle_count[0] != prev_lane_0) {
    prev_lane_0 = vehicle_count[0];
    make_non_zero_0(current_road, prev_lane_0); //change new
  }

  if (vehicle_count[2] != prev_lane_1) {
    prev_lane_1 = vehicle_count[2];
    make_non_zero_1(current_road, prev_lane_1); //change new
  }
}

void publish_to_broker(int road_number, int lane_number, int count) {
  char msg[20];
  sprintf(msg, "road_%d/lane_%d", road_number, lane_number);
  char c[20];

  String(count).toCharArray(c, 20);
  client.publish(msg, c);
  //delay(100);
}

void make_non_zero_0(int road, int count ) {
  if (count <= 0) {
    publish_to_broker(road, 0, 0);
    vehicle_count[0] = 0;
    count = 0;
  } else {
    publish_to_broker(road, 0, count);
  }
  Serial.printf("Published from lane_0, count= %d \n", count);
}


void make_non_zero_1(int road, int count ) {
  if (count <= 0) {
    publish_to_broker(road, 1, 0);
    vehicle_count[2] = 0;
    count = 0;
  } else {
    publish_to_broker(road, 1, count);
  }
  Serial.printf("Published from lane_1, count= %d \n", count);
}
