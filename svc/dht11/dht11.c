#include "stdint.h"
#include "stddef.h"
#include "avr/io.h"
#include "dht11.h"
#include "ticp.h"
#include "circularBuffer.h"
#include <avr/interrupt.h>
#include "config.h"


static void newdata(void);
static void triggerend(void);

EVTS_DEF_FUNC(dht11_triggerend,triggerend);
EVTS_DEF_FUNC(dht11_newdata,newdata);


static uint8_t temp,hum;

static uint8_t buf[8];
static int8_t pos=0;
static uint8_t half=0;


#define POS_FIRST_DELAY     0
#define POS_5050_PULSE  (POS_FIRST_DELAY+1)
#define POS_HUMH_BIT7   (POS_5050_PULSE+1)
#define POS_HUMH_BIT0   (POS_HUMH_BIT7+7)
#define POS_HUML_BIT7   (POS_HUMH_BIT0+1)
#define POS_HUML_BIT0   (POS_HUML_BIT7+7)
#define POS_TEMPH_BIT7  (POS_HUML_BIT0+1)
#define POS_TEMPH_BIT0  (POS_TEMPH_BIT7+7)
#define POS_TEMPL_BIT7  (POS_TEMPH_BIT0+1)
#define POS_TEMPL_BIT0  (POS_TEMPL_BIT7+7)
#define POS_CHK_BIT7    (POS_TEMPL_BIT0+1)
#define POS_CHK_BIT0    (POS_CHK_BIT7+7)


void dht11_Init(void)
{
  CircularBufInit(buf,sizeof(buf));
}

static uint8_t evt;
static char idx;

void dht11_Start(uint8_t nEvt)
{
  DDRB |= (1<<PB0);    // set PB0 to output  (high)
  PORTB &= ~(1<<PB0);  // set PB0 to low

  ticp_Config(buf, TICP_CLK_DIV_64 | TICP_EDGE_FALL_START, evts_DefToNr(dht11_newdata));//|TICP_EDGE_BOTH);
  evt=nEvt;

  pos=POS_FIRST_DELAY;

#if 1
  evts_post(evts_DefToNr(dht11_triggerend));
#else
  triggerend();
#endif
}

static void triggerend(void)
{
  DDRB &= ~(1<<PB0);   // set PB0 to input
//  PORTB |= (1<<PB0);   // set PB0 pullup enabled
  PORTB &= ~(1<<PB0);  // set PB0 to low
}


static void newdata(void)
{
  
  while (pos<=POS_CHK_BIT0)
  {
    uint8_t v=CircularBufRead(buf);
    uint8_t bit=0;
    
    if (v==0)
      return;
      
    if (v>half)
      bit=1;
    
    if (pos==POS_FIRST_DELAY)
    {
      //first delay
    }
    else if (pos==POS_5050_PULSE)
    {
      half=v/2;  //50-50 pulse
    }
    else if (pos<=POS_HUMH_BIT0)
    {
      hum=(hum<<1)+bit;
    }
    else if (pos<=POS_HUML_BIT0)
    {
      
    }
    else if (pos<=POS_TEMPH_BIT0)
    {
      temp=(temp<<1)+bit;
    }
    else if (pos<=POS_TEMPL_BIT0)
    {
      
    }
    else if (pos<POS_CHK_BIT0)
    {
      
    }
    else
    {
      ticp_Config(NULL, TICP_CLK_NONE,0);
      evts_post(evt);
    }
    pos++;

  }  
}


signed char dht11_RecvTemperature(void)
{
  return temp;
}

signed char dht11_RecvHumidity(void)
{
  return hum;
}






