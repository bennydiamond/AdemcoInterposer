# AdemcoInterposer
Interposer board to capture sensor signals for too old Ademco alarm panel. 
My panel is and Ademco 4110MX which does not support ECP keypad protocol. It uses non-addressable keypads. 6 zones base.
Arduino input pins are just wired to the zone inputs of the alarm panel to capture state values and relay them to Home Assistant.

# Hardware
Uses an Arduino nano V3 from DFRobot (ATMega328, 3.3V) and a W5500 Lite ethernet board.
Voltage divider for sensor inputs which run on 12.6Vdc; high impedance as the panel itself isn't all that high.
Dual PC817 optocoupler board for muxing sensor pins (more details below).

# Libraries
[ArduinoHA](https://github.com/dawidchyrzynski/arduino-home-assistant) which is a fantastic tool to get you up and running fast. Use a lot of ressource though but I was able to managed to spawn 10 binary sensor instances.


# Quirks
This project is very much tailored to my needs.
I have a weird setup where 2 zones (zones 2 and 4) had 3 sensors each bundled together in series each.
I decoupled those 2 bundles and fed them individually each to a GPIO with internal pullup. For these, no voltage divider is required.
These 2 bundles of 3 sensors are then muxed back internally together (respectively) in the Arduino and feed back to the panel to ensure alarm panel is working like before. 
They are passed to a dual optocoupler board that toggles the zone input of the panel depending on the sensor states. 

I also have one zone where the reed switch is normally open rather than normally closed.