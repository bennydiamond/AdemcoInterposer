#include "strings.h"

char const String_HelloWorld_[] PROGMEM            = "Hello World!";
char const String_EthernetInit_[] PROGMEM          = "Ethernet init";
char const String_ClientID_[] PROGMEM              = "Ademco";

char const * const String_table[String_IndexCount] PROGMEM = 
{
  String_HelloWorld_,
  String_EthernetInit_,
  String_ClientID_,
};

void getString (StringIndex_t idx, char *out) 
{
  strcpy_P(out, (char *)pgm_read_ptr(&(String_table[idx])));  // Necessary casts and dereferencing, just copy.
}