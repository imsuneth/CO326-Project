#include <WiFi.h>
#include <PubSubClient.h>
#include <Ethernet.h>

enum CLSTATE {GO = 1, STOP = 0};
int led_pins[16] = {16, 26, 15, 4, 25, 14, 27, 17, 5, 18, 23, 19, 13, 33, 21, 22};
int cl_status[8] = {0, 0, 0, 0, 0, 0, 0, 0};
String nodes_status = "online";

//WiFi
const char* ssid = "Dialog 4G wish";
const char* password = "wishez1234";

//MQTT Broker IP address
const char* mqtt_server = "192.168.1.2";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to");
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

void cl_go(int road, int lane) {
  digitalWrite(led_pins[4 * road + 2 * lane + 0], LOW);
  digitalWrite(led_pins[4 * road + 2 * lane + 1], HIGH);
}

void cl_stop(int road, int lane) {
  digitalWrite(led_pins[4 * road + 2 * lane + 0], HIGH);
  digitalWrite(led_pins[4 * road + 2 * lane + 1], LOW);
}

void cl_stop_all() {
  for (int i = 1; i < 16; i += 2) {
    digitalWrite(led_pins[i], LOW);
    digitalWrite(led_pins[i-1], HIGH);
  }
//  for (int i = 0; i < 16; i += 2) {
//    digitalWrite(led_pins[i], HIGH);
//  }
}

void cl_update() {
  for (int i = 0; i < 8; i++) {
    if (cl_status[i] == '1') {
      cl_go(i / 2, i % 2);
    } else if (cl_status[i] == '0') {
      cl_stop(i / 2, i % 2);
    }
  }

}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  if (String(topic) == "esp32/cl_status") {
    for (int i = 0; i < length; i++) {
      Serial.print((char)message[i]);
      cl_status[i] = (char)message[i];
    }
    Serial.println();
    //Update color lights
    cl_update();
  }

  if (String(topic) == "willTopic") {
    nodes_status = "";
    for (int i = 0; i < length; i++) {
      Serial.print((char)message[i]);
      nodes_status += (char)message[i];
    }
    Serial.println();
    
    if (nodes_status ==  "offline") {
      time_based2();
    }
  }

}

void reconnect() {

  // Loop until we're reconnected
  //while (!client.connected()) {
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (client.connect("ESP32Client")) {
    Serial.println("connected");
    // Subscribe
    client.subscribe("esp32/cl_status");
    client.subscribe("willTopic");
    client.subscribe("time");

  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
    // Wait 5 seconds before retrying
    delay(100);
  }
  //}
}

void time_based() {
  Serial.println("Time based controlling started!");
  int road = 0;

  while (true) {

    Serial.print("Open road:");
    Serial.println(road);
    //cl_stop_all();
    cl_go(road, 0);
    cl_go(road, 1);

    reconnect();
    delay(10);
    cl_stop(road, 0);
    cl_stop(road, 1);
    road = (road + 1) % 4;



    if (client.connected()) {
      break;
    }
  }
}
void time_based2() {
  Serial.println("Time based controlling started!");
  int road = 0;

  while (true) {

    Serial.print("Open road:");
    Serial.println(road);
    //cl_stop_all();
    cl_go(road, 0);
    cl_go(road, 1);

    delay(5000);
    cl_stop(road, 0);
    cl_stop(road, 1);
    road = (road + 1) % 4;
    client.loop();
    if (nodes_status == "online") {
      break;
    }

  }
}

void setup() {

  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
//  client.setKeepAlive(5);
//  client.setSocketTimeout(1);

  // set OUTPUTs
  for (int i = 0; i < 16; i++) {
    pinMode(led_pins[i], OUTPUT);
  }
  cl_stop_all();

  if (!client.connected()) {
    reconnect();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  if (!client.connected()) {
    //Switch to time based mode
    time_based();
  }
  client.loop();



}
