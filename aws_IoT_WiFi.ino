/*
  AWS IoT WiFi

  This sketch securely connects to an AWS IoT using MQTT over WiFi.
  It uses a private key stored in the ATECC508A and a public
  certificate for SSL/TLS authetication.

  It publishes a message every 5 seconds to arduino/outgoing
  topic and subscribes to messages on the arduino/incoming
  topic.

  The circuit:
  - Arduino MKR WiFi 1010 or MKR1000

  The following tutorial on Arduino Project Hub can be used
  to setup your AWS account and the MKR board:

  https://create.arduino.cc/projecthub/132016/securely-connecting-an-arduino-mkr-wifi-1010-to-aws-iot-core-a9f365

  This example code is in the public domain.
*/

#include <Arduino_JSON.h>
#include <assert.h>

#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h> // change to #include <WiFi101.h> for MKR1000
#include "arduino_secrets.h"

/////// Enter your sensitive data in arduino_secrets.h
const char ssid[]        = SECRET_SSID;
const char pass[]        = SECRET_PASS;
const char broker[]      = SECRET_BROKER;
const char* certificate  = SECRET_CERTIFICATE;

WiFiClient    wifiClient;            // Used for the TCP socket connection
BearSSLClient sslClient(wifiClient); // Used for SSL/TLS connection, integrates with ECC508
MqttClient    mqttClient(sslClient);

// unsigned long lastMillis = 0;

void setup() {
  // Serial.begin(115200);
    Serial.begin(9600);
    delay(1500);
    // while (!Serial);     //comment out this one to run this scripts without open up serial monitor

  if (!ECCX08.begin()) {
    Serial.println("No ECCX08 present!");
    while (1);
  }

  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);

  // Optional, set the client id used for MQTT,
  // each device that is connected to the broker
  // must have a unique client id. The MQTTClient will generate
  // a client id for you based on the millis() value if not set
  //
  // mqttClient.setId("clientId");

  // Set the message callback, this function is
  // called when the MQTTClient receives a message
  mqttClient.onMessage(onMessageReceived);

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    // MQTT client is disconnected, connect
    connectMQTT();
  }

  // poll for new MQTT messages and send keep alives
  mqttClient.poll();

  // publish a message roughly every 5 seconds.
  // if (millis() - lastMillis > 5000) {
  //   lastMillis = millis();

  //   publishMessage();
  // }
}

unsigned long getTime() {
  // get the current time from the WiFi module  
  return WiFi.getTime();
}

void connectWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  Serial.print(" ");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the network");
  Serial.println();

  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
}

void connectMQTT() {
  Serial.print("Attempting to MQTT broker: ");
  Serial.print(broker);
  Serial.println(" ");

  while (!mqttClient.connect(broker, 8883)) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the MQTT broker");
  Serial.println();

  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);
  digitalWrite(LED_BUILTIN, HIGH);

  // subscribe to a topic
  mqttClient.subscribe("arduino/incoming");
}

// void publishMessage() {
//   Serial.println("Publishing message");

//   // send message, the Print interface can be used to set the message contents
//   mqttClient.beginMessage("arduino/outgoing");
//   mqttClient.print("hello World! ");
//   mqttClient.print(millis());
//   mqttClient.endMessage();

//   digitalWrite(LEDR, HIGH);
//   delay(300);
//   digitalWrite(LEDR, LOW);
// }

//SELECT topic(2) as mqttMessage FROM 'sdk/test/js'
//SELECT color AS rgb FROM 'topic/subtopic' WHERE temperature > 50
//SELECT color as RGB FROM 'sdk/test/js'
//SELECT "123,43,223" as RGB FROM 'sdk/test/js'

void onMessageReceived(int messageSize) {
  String json;

  digitalWrite(LEDB, HIGH);
  delay(300);
  digitalWrite(LEDB, LOW);

  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

//    https://forum.arduino.cc/t/how-to-convert-mqtt-stream-into-a-string/895821
//    https://forum.arduino.cc/t/problems-receiving-mqtt-message/698944/2
  while (mqttClient.available()) {
    // Serial.print((char)mqttClient.read());
    char j = mqttClient.read();
    json += j;
  }
  Serial.println(json);
  JSONVar myObject = JSON.parse(json);

  // Serial.println(JSON.typeof(myObject)); // prints: object
  // Serial.println((const char*) myObject["RGB"]);

  int red   = atoi((const char*) myObject["RGB"]);
  int green = atoi(strchr((const char*) myObject["RGB"], ',') + 1);
  int blue  = atoi(strrchr((const char*) myObject["RGB"], ',') + 1);

  Serial.println(red);
  Serial.println(green);
  Serial.println(blue);

  Serial.println();

//COMMON_ANODE
  analogWrite(LEDR, 255-red);
  analogWrite(LEDG, 255-green);
  analogWrite(LEDB, 255-blue);
  
}

