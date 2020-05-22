 #include <ESP8266WiFi.h>
 #include <PubSubClient.h>
#define BUTTON  D4    // the number of the pushbutton pin
#define RELAY  D3     // the number of the relay pin
int pushed = 0;
int lightON = 0;
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

// Update these with values suitable for your network.

const char* ssid = "bangsad";
const char* password = "william12345";
const char* mqtt_server = "broker.mqttdashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
//char msg[MSG_BUFFER_SIZE];
const char* msgHidup = "terbuka";
const char* msgMati = "terkunci";  
int value = 0;

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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(RELAY, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    Serial.println(msgHidup);
    client.publish("outTopic", msgHidup);
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(RELAY, LOW);  // Turn the LED off
    Serial.println(msgMati);
    client.publish("outTopic", msgMati);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    WiFi.mode(WIFI_STA);
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("solenoid");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(RELAY, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

//  unsigned long now = millis();
//  if (now - lastMsg > 2000) {
//    lastMsg = now;
//    ++value;
//    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
//    //Serial.print("Publish message: ");
//    //Serial.println(msg);
//    client.publish("outTopic", msg);
//  }
}
