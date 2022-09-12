#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID ""//Name of wifi network
#define WIFI_PASS "12345678"//Password of wifi netword

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "" //Adafruit name
#define MQTT_PASS "" //Adafruit AIO key

int led = 2;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe Batsignal = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/Batsignal");
void setup()
{
  Serial.begin(9600);

  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi>");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  //The relay is active-low
  digitalWrite(LED_BUILTIN, LOW);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(">");
    delay(50);
  }

  Serial.println("OK!");

  //Subscribe to the Batsignal topic
  mqtt.subscribe(&Batsignal);

  pinMode(led, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(led, HIGH);

}

void loop()
{
 digitalWrite(led,HIGH);

  //Connect/Reconnect to MQTT
  MQTT_connect();

  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &Batsignal)
    {
      //Print the new value to the serial monitor
      Serial.print("LED: ");
      Serial.println((char*) Batsignal.lastread);

      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) Batsignal.lastread, "ON"))
      {
        digitalWrite(led, LOW);
        delay(25000);  //Change this value to adjust ON time of LED, default is 2 min
        digitalWrite(led,HIGH);
      }
      else if (!strcmp((char*) Batsignal.lastread, "OFF"))
      {
        digitalWrite(led, HIGH);
       
      }
     
    }
  }

}


void MQTT_connect()
{

  //  // Stop if already connected
  if (mqtt.connected() && mqtt.ping())
  {
    //    mqtt.disconnect();
    return;
  }

  int8_t ret;

  mqtt.disconnect();

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      ESP.reset();
    }
  }
  Serial.println("MQTT Connected!");
}
