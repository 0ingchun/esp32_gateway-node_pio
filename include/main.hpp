#include <Arduino.h>
#include <WiFi.h>

#include "HTTPClient.h"
#include "PubSubClient.h"
#include "ArduinoJson.h"

#define USE_MULTCORE 1

char buffer[18];
char character = ','; //终止字符
int numdata=0;

#define LED_BUILTIN 2

const char* ssid = "White-House";
const char* password = "qazwsxedcrfv123!";
const char* mqtt_server = "192.168.2.200";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(500) //50
char msg[MSG_BUFFER_SIZE];
int value = 0;

String clientId = "ESP32Client-GatWay-Node" + String(random(0xffff), HEX), clientUser = "user32s1", clientPasswd = "88888888";