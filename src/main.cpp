#include <Arduino.h>
#include <avr/wdt.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHA.h>
#include "SensorsReader.h"

#define VERSION "1.0.0"
#define DEVICE_NAME "AdemcoInterposer"
#define DEVICE_MANUFACTURER "DIY"
#define DEVICE_MODEL "DFRobot Nano v3"
#define MAC_ADDR_LENGTH (6)
char const String_ClientID[] PROGMEM = "Ademco";
#define CLIENTID_BUFFER_LENGTH sizeof(String_ClientID)
auto const MaxDeviceLimit = 10;

static boolean const willRetain = true;


// MQTT Broker
IPAddress const MQTT_SERVER(192, 168, 0, 250);
char const * const MQTTUserPass = "sonoff";

// The IP address of the Arduino
uint8_t const macFlash[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };

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
  Serial.println(F("Hello World!"));

  uint8_t macStack[MAC_ADDR_LENGTH];
  for(byte i = 0; i < MAC_ADDR_LENGTH; i++)
  {
    macStack[i] = pgm_read_byte_near(macFlash + i);
  }

  Ethernet.begin(const_cast<uint8_t *>(macStack), ip, dns, gateway, subnet);
  Serial.println(F("Ethernet init"));

  // Let network have a chance to start up
  delay(1500);

  byte stackBuf[CLIENTID_BUFFER_LENGTH];
  for (byte i = 0; i < CLIENTID_BUFFER_LENGTH; i++) 
  {
    stackBuf[i] = pgm_read_byte_near(String_ClientID + i);
  }
  device.setUniqueId(stackBuf, strlen(reinterpret_cast<char *>(stackBuf))); // the input array is cloned internally
  device.setName(F(DEVICE_NAME)); // content is not copied
  device.setManufacturer(F(DEVICE_MANUFACTURER));      // content is not copied
  device.setSoftwareVersion(F(VERSION)); // content is not copied
  device.setModel(F(DEVICE_MODEL)); // content is not copied

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
