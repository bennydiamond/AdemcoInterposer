#define LINE_BREAK "\r\n"
#include <Arduino.h>

#pragma once

#define StringTable_SingleStringMaxLength (33) // Make sure this is actually true before compiling.
#define StringTable_MQTTStringMaxLength (24) // Make sure this is actually true before compiling.
typedef enum
{
  String_HelloWorld = 0,
  String_EthernetInit,
  String_ClientID,

  String_IndexCount
} StringIndex_t;
#define MQTTBrokerrPass MQTTBrokerrUser
extern char const * const String_table[String_IndexCount] PROGMEM;


void getString (StringIndex_t idx, char *out);