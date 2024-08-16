#include <Arduino.h>
#include <avr/wdt.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHA.h>
#include "SensorsReader.h"
#include "strings.h"

char const * const version = "1.0.0";
auto const MaxDeviceLimit = 10;

static boolean const willRetain = true;

// MQTT Broker
IPAddress const MQTT_SERVER(192, 168, 0, 250);
char const * const MQTTUserPass = "sonoff";

// The IP address of the Arduino
uint8_t const mac[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };

// Remove remarks and alter if you want a fixed IP for the Arduino
IPAddress const ip(192, 168, 1, 201);
IPAddress const dns(192, 168, 0, 11);
IPAddress const gateway(192, 168, 0, 3);
IPAddress const subnet(255,255,254,0);

// Ethernet Initialisation
EthernetClient ethClient;
HADevice device;
HAMqtt mqtt(ethClient, device, MaxDeviceLimit);

SensorsReader sensorsReader;

void setup() 
{
  // Start Serial
  Serial.begin(115200);
  char szString0[StringTable_SingleStringMaxLength];
  getString(String_HelloWorld, szString0);
  Serial.println(szString0);

  Ethernet.begin(const_cast<uint8_t *>(mac), ip, dns, gateway, subnet);
  getString(String_EthernetInit, szString0);
  Serial.println(szString0);

  // Let network have a chance to start up
  delay(1500);

  getString(String_ClientID, szString0);
  device.setUniqueId((byte *)szString0, strlen(szString0)); // the input array is cloned internally
  device.setName("AdemcoInterposer"); // content is not copied
  device.setManufacturer("DIY");      // content is not copied
  device.setSoftwareVersion(version); // content is not copied
  device.setModel("DFRobot Nano v3"); // content is not copied

  sensorsReader.init();

  device.enableSharedAvailability();
  device.enableLastWill();

  mqtt.begin(MQTT_SERVER, 1883, MQTTUserPass, MQTTUserPass);

  //watchdog timer with 8 Seconds time out
  wdt_enable(WDTO_8S);
}

void loop() 
{
  sensorsReader.loop();
  Ethernet.maintain();
  mqtt.loop();

  if(device.isAvailable() && mqtt.isConnected())
  {
    wdt_reset();
  }
}
