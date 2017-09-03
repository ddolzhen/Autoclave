#ifndef Morse_h
#define Morse_h

#include "Arduino.h"
class Status {
 
public:
   Status();
   unsigned int bit1 : 1;//valve1 / 1 / 1
   unsigned int bit2 : 1;//valve2 / runstp / level2
     unsigned int bit3 : 1;//valve3 / mode bit1 / level3
     unsigned int bit4 : 1;//valve4 / mode bit2 / burner
     unsigned int bit5 : 1;//valve5 / mode bit3
   unsigned int bit6 : 1;//valve6 / stage bit1 
   unsigned int bit7 : 1;//level  / stage bit2
   unsigned int bit8 : 1;//pump   / stage bit3
   char getByte();
   void setBytes(byte code);
   void setBit(byte pos,byte onoff);
};


#endif
