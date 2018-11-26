#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include "config.h"

const char *wifi_ssid = WIFI_SSID;
const char *wifi_pass = WIFI_PASS;

uint8_t sensor_light_pin = 4;
uint8_t sensor_soil_pin = 5;

unsigned long current_time = millis();
unsigned int sensor_update_time = current_time;

int sensor_light_value = 0;
int sensor_soil_value = 0;

WiFiClient client;
MicroGear microgear(client);

void handleMicrogearConnect(char *attribute, uint8_t *msg, unsigned int msglen)
{
  Serial.println("[#] Connected to NETPIE...");
}

void setup()
{
  microgear.on(CONNECTED, handleMicrogearConnect);

  Serial.begin(9600);
  Serial.println();
  Serial.println("[#] Starting");

  // Setup PIN
  pinMode(sensor_light_pin, INPUT);
  pinMode(sensor_soil_value, INPUT);

  // Connect to WiFi
  wifi_connect();

  // Init MicroGear
  microgear.init(KEY, SECRET, ALIAS);
  microgear.connect(APPID);
}

void loop()
{
  // Verify that device still connected to WiFi
  if (!WiFi.isConnected())
  {
    wifi_connect();
  }

  // Microgear
  if (microgear.connected())
  {
    microgear.loop();

    // Update loop
    if (current_time - sensor_update_time >= 1000)
    {
      // Publish Light sensor value
      sensor_light_value = digitalRead(sensor_light_pin);
      Serial.print("Light: ");
      Serial.print(sensor_light_value);
      Serial.println();
      microgear.publish("/sensor/light", sensor_light_value);

      // Publish Soil sensor value
      sensor_soil_value = digitalRead(sensor_soil_pin);
      Serial.print("Soil: ");
      Serial.print(sensor_soil_value);
      Serial.println();
      microgear.publish("/sensor/soil", sensor_soil_value);
      sensor_update_time = current_time;
    }
  }
  else
  {
    Serial.println("connection lost, reconnect...");
    if (current_time - sensor_update_time >= 5000)
    {
      microgear.connect(APPID);
      sensor_update_time = current_time;
    }
  }

  current_time = millis();
}

// WiFi Connect
void wifi_connect()
{
  wl_status_t wifi_status = WiFi.begin(wifi_ssid, wifi_pass);
  if (wifi_status)
  {
    Serial.println("[#] Connecting to WiFi");

    // Block until WiFi connected
    while (wifi_status != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
      wifi_status = WiFi.status();
    }
  }

  Serial.println();
  Serial.println("[#] Connected");
  Serial.print("[#] IP Address: ");
  Serial.println(WiFi.localIP());
}