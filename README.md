# Project Chicken Door
Project to make a mechanism that can open and close each day at predefined times. The project utilizes the following components:
- Mechanics and power electronics:
     - An 100kg AC lift.
     - Bunch of cables that can handle the lift amp draw.
     - Two [AC limit switches](https://www.banggood.com/5pcs-AC-250V-5A-Adjustable-Rotary-Roller-Lever-Limit-Switch-p-1112331.html?rmmds=search&cur_warehouse=CN) that can handle the lift amp draw.
     - Some electronics junction boxes for safe cable management
- Fine electronics:
     - [Arduino Nano](https://www.banggood.com/RobotDyn-Nano-V3_0-CH340ATmega328P-16MHz-Assembled-Version-For-Arduino-p-1128921.html?rmmds=search&cur_warehouse=CN) or Uno.
     - [DS3231 clock module](https://www.banggood.com/DS3231-AT24C32-IIC-High-Precision-Real-Time-Clock-Module-For-Arduino-p-81066.html?rmmds=search&cur_warehouse=UK)
     - [Keypad shield for Arduino](https://www.banggood.com/Keypad-Shield-Blue-Backlight-For-Arduino-Robot-LCD-1602-Board-p-79326.html?rmmds=search&cur_warehouse=CN)
     - [4x relays](https://www.banggood.com/5V-4-Channel-Relay-Module-For-Arduino-PIC-ARM-DSP-AVR-MSP430-Blue-p-87987.html?rmmds=search&cur_warehouse=CN)

The lift should be connected so that the limit switches prevent the lift from opening/closing the door beyond the chosen points, and the relay should be connected to act as a second remote preferably reusing the capacitor of the actual remote. The way I connected it can be seen below.

![Figure 1: Image of wiring of lift, limit switches, and relays.]()
