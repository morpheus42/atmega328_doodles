
#include "avr/io.h"
#include "dht11.h"
#include "ticp.h"
#include "circularBuffer.h"
#include <avr/interrupt.h>

#include "contiki.h"


PROCESS(dht11collect_process, "dht11collect");
AUTOSTART_PROCESSES(&dht11collect_process);



//static char idx;
static char sensor[5];

static unsigned char buf[50];

void dht11_init(void)
{
  DDRB |= (1<<PB2);
  CircularBufInit(buf,sizeof(buf));
  process_start(&dht11collect_process,NULL);
}

void dht11_start(void)
{
//  process_start(&dht11collect_process,NULL);

  DDRB |= (1<<PB0);    // set PB0 to output  (high)
  PORTB &= ~(1<<PB0);  // set PB0 to low

  process_post(&dht11collect_process, PROCESS_EVENT_INIT, NULL);
  ticp_config(buf, TICP_CLK_DIV_64 | TICP_EDGE_FALL_START );//|TICP_EDGE_BOTH);

  {
    long volatile i=0x8000;
    while(i>0)
      i--;
  }

  DDRB &= ~(1<<PB0);   // set PB0 to input
  PORTB |= (1<<PB0);   // set PB0 to high
}


PROCESS_THREAD(dht11collect_process, ev, data)
{
  PROCESS_BEGIN();
  static char idx;
  char * p;
  
  idx=0;

  while (idx<(sizeof(sensor)*8+1))
  {
    char x;
    signed short v=CircularBufRead(buf);

    if (v<0)
    {
      PROCESS_PAUSE();

      if (idx==(sizeof(sensor)*8-1))
        idx=(sizeof(sensor)*8+1);
    }
    else
    {
      
      PORTB |= (1<<PB2);

      x=v;
      
      if (x<15)
      {
        x=0;
      }
      else if (x<25)
      {
        x=2;
      }
      else if (x<35)
      {
        x=3;
      }
      else if (x<50)
      {
        x=0;
        idx=0;
      }
      else
      {
        x=0;
      }
      
      if (x&2)
      {
        if (idx<(sizeof(sensor)*8))
        {
          unsigned char m;
          p=&sensor[idx>>3];
          
          m = (0x80>>(idx&7));
          if (x&1)
            *p |= m;
          else
            *p &= ~m;
          
          idx++;
        }
        if (idx>=(sizeof(sensor)*8)-1)
        {
          ticp_config(NULL, TICP_CLK_NONE);
        }
        
      }
      PORTB &= ~(1<<PB2);
    }
    
  }
  
  PROCESS_END();
}


signed char dht11_latest_temperature(void)
{
  return sensor[2];
}

signed char dht11_latest_humidity(void)
{
  return sensor[0];
}










