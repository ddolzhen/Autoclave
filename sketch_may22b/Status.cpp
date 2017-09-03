
#include "Status.h"

Status::Status()
{
 this->bit1=1;
 this->bit2=1;
 this->bit3=1;
 this->bit4=1;
 this->bit5=1;
 this->bit6=1;
 this->bit7=1;
 this->bit8=1;
}

char Status::getByte()
{
  return((char)(bit1+bit2*2+bit3*4+bit4*8+bit5*16+bit6*32+bit7*64+bit8*128)); 
}

void Status::setBytes(byte code)
{
  if(code&1)
  {
    bit1=1; 
  }else
  {
    bit1=0;
  }

  
  if(code&2)
  {
    bit2=1;
  }else
  {
    bit2=0;
  }

  
  if (code&4)
  {
    bit3=1;
  }else
  {
    bit3=0;
  }


  
  if (code&8)
  {
    bit4=1;
  }else
  {
    bit4=0;
  }

  if(code&16)
  {
    bit5=1;
  }else
  {
    bit5=0;
  }

  if (code&32)
  {
    bit6=1;
  }else
  {
    bit6=0;
  }


  if(code&64)
  {
    bit7=1;
  }else
  {
    bit7=0;
  }


  if(code&128)
  {
    bit8=1;
  }else
  {
    bit8=0;
  }
  
}




void Status::setBit(byte pos,byte onoff)
{
  char data=this->getByte();
  if (onoff)
  {
    data |= 1 << pos;
  }else
  {
    data &= ~(1 << pos);
  }
  //data^= (-onoff ^ data) & (1 << pos);
  this->setBytes(data);
}


