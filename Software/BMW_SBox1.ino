/*
BMW Sbox Test Code V1
Special thanks to https://github.com/crasbe for figuring out the CRC.
Assumes Sbox on CAN0.

Copyright 2023
Damien Maguire
*/


#include "CRC.h"
#include <due_can.h>  
#include <due_wire.h> 
#include <DueTimer.h>  
#include <Metro.h>

#define SerialDEBUG SerialUSB
template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } //Allow streaming
CAN_FRAME outframe;  //A structured variable according to due_can library for transmitting CAN data.
CAN_FRAME inFrame;    //structure to keep inbound inFrames

int led = 13;         //onboard led for diagnosis
float Version=1.01;

Metro timer_20ms=Metro(20); 
Metro timer_1000ms=Metro(1000);
uint8_t can_data[8];
uint8_t can_ctr=0;
uint8_t can_bytes[3]={0x85,0x1,0xFF};
bool Display=false;
float BattCurrent=0;
float BattVoltage=0;
float OutputVoltage=0;

void setup()
{
  SerialDEBUG.begin(115200);
  Can0.begin(CAN_BPS_500K);   // Charger CAN
  Can1.begin(CAN_BPS_500K);   // External CAN
  Can0.watchFor();
  Can1.watchFor();
  pinMode(led, OUTPUT);
}


void loop()
{
if(timer_20ms.check()) Msgs20ms();
if(timer_1000ms.check()) Tasks1000ms(); 
checkforinput();
CheckRecCan();
}

void Tasks1000ms()
{
if(Display) ShowRecData();

//can_bytes[0]++;
//can_bytes[2]++; 
//can_bytes[1]++; 
//if(can_bytes[1]>0xF) can_bytes[1]=0;
//if(can_bytes[0]>0xF) can_bytes[0]=0;
}

void Msgs20ms()
{
        outframe.id = 0x100;            
        outframe.length = 4;            
        outframe.extended = 0;          
        outframe.rtr=1;                 
        outframe.data.bytes[0]=can_bytes[0];
        outframe.data.bytes[1]=(can_ctr<<4|can_bytes[1]);//counter in lower nibble
        outframe.data.bytes[2]=can_bytes[2];
        outframe.data.bytes[3]=0x00;    //CRC byte
        outframe.data.bytes[4]=0x00;//all 0 from here for crc calc purposes
        outframe.data.bytes[5]=0x00;
        outframe.data.bytes[6]=0x00;
        outframe.data.bytes[7]=0x00; 
        for(uint8_t i=0; i<8; i++)
        {
               can_data[i]=outframe.data.bytes[i];
        }
       outframe.data.bytes[3]=crc8(can_data, 8, 0x31, 0x00, 0x00, true, true);
       can_ctr++;
       if(can_ctr>0xE) can_ctr=0;
        Can0.sendFrame(outframe);    


        outframe.id = 0x300;            
        outframe.length = 4;            
        outframe.extended = 0;          
        outframe.rtr=1;                 
        outframe.data.bytes[0]=0xFF;//Seems to control the iso test relay somehow.
        outframe.data.bytes[1]=0xFE;//must be 0xFE to enable contactors. Set to 0xFF to disable
        outframe.data.bytes[2]=0xFF;;
        outframe.data.bytes[3]=0xFF;
        Can0.sendFrame(outframe); 

}

void checkforinput()
{ 
  //Checks for keyboard input from Native port 
   if (SerialDEBUG.available()) 
     {
      int inByte = SerialDEBUG.read();
      switch (inByte)
         {
          case 'n':            
           can_bytes[0] = 0x62;
           SerialDEBUG.println("Neg contactor activated");
            break;
  
          case 'p':    
           can_bytes[0] = 0x0A;
           SerialDEBUG.println("Pos contactor activated");
            break;
            
          case 'r':    
           can_bytes[0] = 0x86;
           SerialDEBUG.println("Prech contactor activated");       
            break;

          case 'P':    
           can_bytes[0] = 0xA6;
           SerialDEBUG.println("Prech and Neg contactors activated");       
            break;   

          case 'A':    
           can_bytes[0] = 0xAA;
           SerialDEBUG.println("All contactors activated");       
            break; 

          case 'o':    
           can_bytes[0] = 0x00;
           SerialDEBUG.println("All contactors deactivated");       
            break;     

          case 'D':     
              Display=!Display;
            break;

          case '?':     //Print a menu describing these functions
              printMenu();
            break;
         
          }    
      }
}

void printMenu()
{
   SerialDEBUG<<"\f\n=========== EVBMW BMW Sbox Controller Version "<<Version<<" ==============\n************ List of Available Commands ************\n\n";
   SerialDEBUG<<"  ?  - Print this menu\n ";
   SerialDEBUG<<"  p -  Activate positive contactor\n";
   SerialDEBUG<<"  n -  Activate negative contactor\n";
   SerialDEBUG<<"  r  - Activate precharge relay \n ";
   SerialDEBUG<<"  P  - Close Negative Contactor and Precharge relay\n ";
   SerialDEBUG<<"  A  - Close All\n";
   SerialDEBUG<<"  o  - Deactivcate all contactors and precharge relay\n ";
   SerialDEBUG<<"  D  - Toggle data display from SBox\n ";
   SerialDEBUG<<"**************************************************************\n==============================================================\n\n";
   
}

void CheckRecCan()
{
  if(Can0.available())
  {
    Can0.read(inFrame);

    if(inFrame.id ==0x200)
    {
      BattCurrent=((inFrame.data.bytes[2]<<16)| (inFrame.data.bytes[1]<<8)| (inFrame.data.bytes[0]))-12;

    }

   else if(inFrame.id ==0x210)
    {
      BattVoltage=((inFrame.data.bytes[2]<<16)| (inFrame.data.bytes[1]<<8)| (inFrame.data.bytes[0]))*0.001;

    }  

   else if(inFrame.id ==0x220)
    {
      OutputVoltage=((inFrame.data.bytes[2]<<16)| (inFrame.data.bytes[1]<<8)| (inFrame.data.bytes[0]))*0.001;

    } 
  }
  
}

void ShowRecData()
{

  SerialDEBUG.println("");
  SerialDEBUG.println("***************************************************************************************************");
  SerialDEBUG.print("Battery Current: ");
  SerialDEBUG.print(BattCurrent);
  SerialDEBUG.println("mA");
  SerialDEBUG.print("Battery Voltage: ");
  SerialDEBUG.print(BattVoltage);
  SerialDEBUG.println("V");
  SerialDEBUG.print("Output Voltage: ");
  SerialDEBUG.print(OutputVoltage);
  SerialDEBUG.println("V");
  SerialDEBUG.println("***************************************************************************************************");

}
