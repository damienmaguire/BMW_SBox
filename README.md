# BMW_SBox
Reverse Engineering of the BMW SBox for use in ev conversion projects.

The contactor box (reffered to as the SBox) is usually found inside the battery packs of BMW Hybrid vehicles e.g. 530e
![2023-04-20 14 08 02](https://user-images.githubusercontent.com/9742117/233445557-2beae562-2a88-45f2-a213-9e4a2ac140c9.jpg)

Inside there are 2 contactors , one precharge relay , precharge resistor as well as voltage and current sensing all tied into a microcontroller:
![2023-04-18 11 13 27](https://user-images.githubusercontent.com/9742117/233445818-180a1b2a-8fc2-435f-9718-fbb93ce8b815.jpg)

Four heavy duty spade terminals provide connection for HV Battery + , - and Output + ,-

Connection of Can data and 12v power is via a white 16 pin connector on the front.

Pins 12 and 14 to +12v

Pin 3 to GND

Pin 1 CANH

Pin 10 CANL

CAN is 500K speed.

Two ids are required to run the SBox and control its contactors:

0x100 and 0x300 at 20ms intervals.

0x100 has a counter and CRC8 and is the main control message.

0x300 can be static.
