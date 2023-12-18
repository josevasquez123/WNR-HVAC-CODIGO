#include <WiFi.h>
#include <PubSubClient.h>
#include <math.h>

#define dacPin 25

// WiFi
const char *ssid = "jose_wifi"; // Enter your WiFi name
const char *password = "abadvalencia1696";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "100.27.0.219";
const char *topic = "damper";
const char *mqtt_username = "billeteronv11";
const char *mqtt_password = "billeterownr123";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

int dacValue;
int damperAngle;

//functions
void wifiConn(void);
void MQTTConn(void);

void setup() {
    // Set software serial baud to 115200;
    Serial.begin(115200);
    
    wifiConn();
    MQTTConn();
    
}

void loop() {
    client.loop();
    dacWrite(dacPin,dacValue);
}


void callback(char* topic, byte* payload, unsigned int length){
  payload[length] = 0;
  String topico=String(topic);
  String msg = String((char *)payload);

  Serial.println(topico);
  Serial.println(msg);
  
  if(topico=="damper"){
    sscanf((char *)payload, "%d", &damperAngle);
    dacValue = 48.0 + 2.179*(float)damperAngle;
  }

}

void wifiConn(void){
  // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
}

void MQTTConn(void){
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("Succesfull Broker connection");
    } else {
        Serial.print("failed with state ");
        Serial.print(client.state());
        delay(2000);
    }
  }
  
  client.subscribe(topic);
}
