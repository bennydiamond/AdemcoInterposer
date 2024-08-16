#include <stdint.h>
#include <ArduinoHA.h>

class SensorsReader
{
public:
    SensorsReader();

    void init (void);
    void fullPublish (void);
    void loop (void);

private:
    static auto const TimerResetValue_ms = 10; 

    void pollInputs (void);
    boolean readInput (uint8_t pinIdx);
    void toggleOutputs (void);
    void toggleOutput (uint8_t outputPin, uint8_t const pinArrayIdx[]);

    HABinarySensor m_zone1;
    HABinarySensor m_zone2;
    HABinarySensor m_zone3;
    HABinarySensor m_zone4;
    HABinarySensor m_zone5;
    HABinarySensor m_zone6;
    HABinarySensor m_zone7;
    HABinarySensor m_zone8;
    HABinarySensor m_zone9;
    HABinarySensor m_zone10;

    uint8_t m_timer;
    uint32_t m_previousMillis;
};