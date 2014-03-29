



#include "mudem.h"


#define MUDEM_START     0x02
#define MUDEM_ESC       0x1B
#define MUDEM_ESC_OFS   0x40


typedef struct
{
  uint8_t channel:3;
  uint8_t esc:1;
  uint8_t pos:1;
  uint8_t valid:1;
}mudem_inst;


static mudem_inst this;

void CallRecv(uint8_t b)
{
  if (this.valid)
  {
    ByteReceived_t * f = mudem_channels[this.channel].pfnByteReceived;
    if (f)
    {
      f(b);
    }
  }
}

void CallCtrl(uint8_t b)
{
  if (this.valid)
  {
    ByteReceived_t * f = mudem_channels[this.channel].pfnControl;
    if (f)
    {
      f(b);
    }
  }
}

void mudem_ByteReceived(uint8_t b)
{
  if (b==MUDEM_START)
  {
    this.pos=0;
    this.esc=0;
  }
  else
  {
    if (b==MUDEM_ESC)
    {
      this.esc=1;
    }
    else
    {
      if (this.esc==1)
      {
        b ^= MUDEM_ESC_OFS;
        this.esc=0;
      }
      if (this.pos==0)
      {
        this.channel = b >> 5;
        if (this.channel<=sizeof(mudem_channels)/sizeof(mudem_channel_t))
        {
          this.valid=1;
        }
        CallCtrl(b);
        this.pos=1;
      }
      else
      {
        CallRecv(b);
      }
    }
  
  
  }

}




