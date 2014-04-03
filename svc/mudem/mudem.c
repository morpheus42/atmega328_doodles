


#include "stddef.h"
#include "mudem.h"


#define MUDEM_START     0x02
#define MUDEM_ESC       0x1B
#define MUDEM_ESC_OFS   0x40


typedef struct
{
  uint8_t chIn:3;
  uint8_t escIn:1;
  uint8_t posIn:1;
  uint8_t validIn:1;
  uint8_t stuff0:2;
  
  uint8_t chOut:3;
  uint8_t escOut:1;
  uint8_t posOut:2;

  uint8_t cntOut;
  uint8_t cOut;
}mudem_inst;


static mudem_inst this;

void CallRecv(uint8_t b)
{
  if (this.validIn)
  {
    ByteReceived_t * f = mudem_channels[this.chIn].pfnByteReceived;
    if (f)
    {
      f(b | MUDEM_DATA);
    }
  }
}

void CallCtrl(uint8_t b)
{
  if (this.validIn)
  {
    ByteReceived_t * f = mudem_channels[this.chIn].pfnByteReceived;
    if (f)
    {
      f(b | MUDEM_CTRL);
    }
  }
}

void mudem_ByteReceived(uint8_t b)
{
  if (b==MUDEM_START)
  {
    this.posIn=0;
    this.escIn=0;
  }
  else
  {
    if (b==MUDEM_ESC)
    {
      this.escIn=1;
    }
    else
    {
      if (this.escIn==1)
      {
        b ^= MUDEM_ESC_OFS;
        this.escIn=0;
      }
      if (this.posIn==0)
      {
        this.chIn = b >> 5;
        if (this.chIn<=sizeof(mudem_channels)/sizeof(mudem_channel_t))
        {
          this.validIn=1;
        }
        CallCtrl(b);
        this.posIn=1;
      }
      else
      {
        CallRecv(b);
      }
    }
  
  }

}



uint16_t mudem_GetOut(void)
{
  ByteTransmit_t * pfnGetOut;
  uint8_t c,ch;
  uint16_t w;

  if (this.escOut)
  { // previous char was escape char
    this.escOut=0;
    return this.cOut | 0x100;
  }

  if (this.posOut==0)
  { // previous char was a start, now send channel
    uint8_t c = this.chOut<<5;
    this.posOut=1;
    return c | 0x100;
  }
    
  if (this.posOut==1)
  { // previous char was channel, now send data
    this.posOut=2;
    return this.cOut | 0x100;
  }
  
  ch=this.chOut;
  #if 0
  if (this.cntOut>63)
  {
    ch++;
    if (ch>=sizeof(mudem_channels)/sizeof(mudem_channel_t))
    {
      ch=0;
    }
    this.cntOut=0;
  }
  #endif
    
  while((pfnGetOut = mudem_channels[ch].pfnByteTransmit)==NULL)
  {
    ch++;
    if (ch>=sizeof(mudem_channels)/sizeof(mudem_channel_t))
    {
      ch=0;
    }
    this.cntOut=0;
  }
    
  w = pfnGetOut();
  if (w)
  { // we got new data to send
    uint8_t c = w;
    if ((c==MUDEM_START) || (c==MUDEM_ESC))
    {
      this.escOut=1;
      c ^= MUDEM_ESC_OFS;
    }
    this.cOut = c;
  
  
    if (ch!=this.chOut)
    { // different channel!!
      this.posOut=0;
      this.escOut=0;
    }
    

  }
  else
  {
    ch++;
    if (ch>=sizeof(mudem_channels)/sizeof(mudem_channel_t))
    {
      ch=0;
    }
    this.chOut=ch;
    return 0;
  }
  this.chOut=ch;


  if (this.posOut==0)
  {
    return  MUDEM_START | 0x100;
  }
  if (this.escOut)
  {
    return MUDEM_ESC |0x100;
  }
  return this.cOut | 0x100;
}



