#include "SensorsReader.h"
#include <Arduino.h>

boolean const NoInvert = false;
boolean const Invert = true;

auto const InputPinsCount = 10;
auto const ZonesMuxPinsCount = 3;

#define DEVICECLASS_DOOR "door"
#define DEVICECLASS_MOTION "motion"
#define DEVICECLASS_WINDOW "window"
typedef struct
{
    uint8_t pin;
    uint8_t pullup;
    boolean invert;
} PinSetup_t;

PinSetup_t const InputPins[InputPinsCount] = 
{
    { PIN_A3, INPUT,        NoInvert }, // Zone 1
    { 4,      INPUT_PULLUP, Invert   }, // Zone 2-1
    { 5,      INPUT_PULLUP, NoInvert }, // Zone 2-2
    { 6,      INPUT_PULLUP, NoInvert }, // Zone 2-3
    { PIN_A2, INPUT,        NoInvert }, // Zone 3
    { 7,      INPUT_PULLUP, NoInvert }, // Zone 4-1
    { 8,      INPUT_PULLUP, NoInvert }, // Zone 4-2
    { 9,      INPUT_PULLUP, NoInvert }, // Zone 4-3
    { PIN_A1, INPUT,        NoInvert }, // Zone 5
    { PIN_A0, INPUT,        NoInvert }  // Zone 6
};

 // Reference index of array members in InputPins[InputPinsCount]
uint8_t const Zone2MuxPinsIdx[ZonesMuxPinsCount] =
{
    1,
    2,
    3
};

 // Reference index of array members in InputPins[InputPinsCount]
uint8_t const Zone4MuxPinsIdx[ZonesMuxPinsCount] = 
{
    5,
    6,
    7
};


SensorsReader::SensorsReader() :
m_zone1("z1"),
m_zone2("z2"),
m_zone3("z3"),
m_zone4("z4"),
m_zone5("z5"),
m_zone6("z6"),
m_zone7("z7"),
m_zone8("z8"),
m_zone9("z9"),
m_zone10("z10"),
m_timer(0),
m_previousMillis(0)
{

}

void SensorsReader::init (void)
{
    for(auto i = 0; i < InputPinsCount; i++)
    {
        pinMode(InputPins[i].pin, InputPins[i].pullup);
    }

    pinMode(PIN_A4, OUTPUT);
    pinMode(PIN_A5, OUTPUT);
    Serial.println(F("SensorsReader init done."));

    m_zone1.setDeviceClass(F(DEVICECLASS_DOOR));
    m_zone1.setName(F("Avant"));

    m_zone2.setDeviceClass(F(DEVICECLASS_DOOR));
    m_zone2.setName(F("Bureau"));

    m_zone3.setDeviceClass(F(DEVICECLASS_MOTION));
    m_zone3.setName(F("Salon"));

    m_zone4.setDeviceClass(F(DEVICECLASS_MOTION));
    m_zone4.setName(F("Entrée"));

    m_zone5.setDeviceClass(F(DEVICECLASS_DOOR));
    m_zone5.setName(F("Patio"));

    m_zone6.setDeviceClass(F(DEVICECLASS_DOOR));
    m_zone6.setName(F("Intérieur Garage RDC"));

    m_zone7.setDeviceClass(F(DEVICECLASS_DOOR));
    m_zone7.setName(F("Intérieur Garage SS"));

    m_zone8.setDeviceClass(F(DEVICECLASS_WINDOW));
    m_zone8.setName(F("Fenêtres SS"));

    m_zone9.setDeviceClass(F(DEVICECLASS_DOOR));
    m_zone9.setName(F("Porte de Coté"));

    m_zone10.setDeviceClass(F(DEVICECLASS_MOTION));
    m_zone10.setName(F("Garage"));


    HABinarySensor* sensors[InputPinsCount] = 
    {
        &m_zone1,
        &m_zone2,
        &m_zone3,
        &m_zone4,
        &m_zone5,
        &m_zone6,
        &m_zone7,
        &m_zone8,
        &m_zone9,
        &m_zone10
    };

    for(auto i = 0; i < InputPinsCount; i++)
    {
        sensors[i]->setCurrentState(digitalRead(InputPins[i].pin));
    }

    m_previousMillis = millis() - 1;
}

void SensorsReader::loop (void)
{
    uint32_t const current = millis();
    if(m_previousMillis != current)
    {
        m_previousMillis = current;

        if(m_timer)
        {
            m_timer--;
        }
        else
        {
            pollInputs();
            toggleOutputs();
            m_timer = TimerResetValue_ms;
        }
    }
}

void SensorsReader::pollInputs (void)
{
    HABinarySensor* sensors[InputPinsCount] = 
    {
        &m_zone1,
        &m_zone2,
        &m_zone3,
        &m_zone4,
        &m_zone5,
        &m_zone6,
        &m_zone7,
        &m_zone8,
        &m_zone9,
        &m_zone10
    };

    for(auto i = 0; i < InputPinsCount; i++)
    {
        sensors[i]->setState(readInput(i));
    }
}

boolean SensorsReader::readInput (uint8_t pinIdx)
{
    boolean value = digitalRead(InputPins[pinIdx].pin);
    if(InputPins[pinIdx].invert == Invert)
    {
        value = !value;
    }

    return value;
}

void SensorsReader::toggleOutputs (void)
{
    toggleOutput(PIN_A5, Zone2MuxPinsIdx);
    toggleOutput(PIN_A4, Zone4MuxPinsIdx);
}

void SensorsReader::toggleOutput (uint8_t outputPin, uint8_t const pinArrayIdx[ZonesMuxPinsCount])
{
    auto i = 0;

    // Check Zone2 mux
    while(i < ZonesMuxPinsCount)
    {
        if(readInput(pinArrayIdx[i])) // Zone open?
        {
            // At least one zone is open
            digitalWrite(outputPin, 0); // disconnect optocoupler, opening up the circuit to the panel
            break;
        }

        i++;
    }
    if(i == ZonesMuxPinsCount) // All zones were closed
    {
        digitalWrite(outputPin, 1); // close optocoupler, closing the circuit to the panel
    }
}
