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

0x100 and 0x300 DLC 4 at 20ms intervals.

0x100 has a counter and CRC8 and is the main control message.

Byte 0 controls contactor function as follows :

0x62 Activates negative contactor

0x0A Activates negative contactor

0x8A Activates positive contactor and precharge relay

0xA6 Activates negative contactor and precharge relay

0x62 Activates negative and positive contactors and precharge relay

0x86 Activates precharge relay only.

Other combinations of bits in byte 0 will cause individual contactors and combinations of contactors and precharge relay to engage.

Byte 1 : Counter in upper nibble running from 0x0 to 0xE. Lower nibble fixed at 0x1.Function not yet investigated.

Byte 2 : fixed at 0xFF. Function not yet investigated.

Byte 3 : CRC8 , Poly 0x31 ,Initial value 0x00, final XOR 0x00 , inverse both. Calculated on length of 8 bytes while message is only 4 bytes long.



0x300 can be static:

0xFF , 0xFE , 0xFF , 0xFF

Information such as Battery voltage, Current, Output voltage etc are provided over CAN making it a very versatile unit. Messages for Ah,kwh,kw etc are being investigated.

Added a simple program that runs on an arduino Due for controling the sbox contactors and reading basic data. Will be integrating with the ZombieVerter VCU project also : 

https://github.com/damienmaguire/Stm32-vcu
