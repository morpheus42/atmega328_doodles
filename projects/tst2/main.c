

#include "config.h"
#include "stddef.h"
#include "board.h"
#include "compdefs.h"
#include "evts.h"
#include "sleep.h"
#include "pkt.h"
#include "sck.h"
#include "pq.h"
//#include "tmrs.h"
#include "ticp.h"
#include "dht11.h"

#include "uart.h"


#ifdef __AVR
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

static void evt1(void);
static void evt2(void);
static void tst3(void);



EVTS_DEF_FUNC(main_evt1,evt1);
EVTS_DEF_FUNC(main_evt2,evt2);
EVTS_DEF_FUNC(main_tst3,tst3);


//LTE_REFGROUP(pevts);
//LTE_ADDENTRY(const short PROGMEM, pevts, nr1) = 0x1111;
//LTE_ADDENTRY(const short PROGMEM, pevts, nr2) = 0x2222;
//LTE_ADDENTRY(short, pevts, nr1 PROGMEM) = 0x1111;
//LTE_ADDENTRY(short, pevts, nr2 PROGMEM) = 0x2222;
//short __attribute((used,__section__("__lte_pevts"))) __lte_pevts_nr2 __attribute__((__progmem__)) = 0x2222;
//const short __attribute((__progmem__)) __lte_pevts_nr3 = 0x3333;



uint8_t r[10];

static sck_id_t fh0,fh2;


static void evt1(void)
{
    char len;
    uint8_t *pS;
    uint8_t *pD;
    int8_t count=sizeof(r)-2;
    
    printf("evt1.\n");
    
    len = sck_ReadFrom(fh0, r, r+2, count); 
    
    if (len>0)
    {
      switch (r[2])
      {
        case 'R':
          //read:
          pS = *(uint16_t*)&r[3];
          pD = &r[6];
          count-=4;
          
          if (r[5]<count)
          {
            count=r[5];
          }
          len=count+4;
          
          printf("Read:[%x]*%x.\n",pS,count);
          
          while (count>0)
          {
            *pD++=*pS++;
            count--;
          }

          break;
          
        case 'W':
          //write:
          pD = *(uint16_t*)&r[3];
          pS = &r[6];
          count-=3;
          
          if (r[5]<count)
          {
            count=r[5];
          }
          len=4;
          
          printf("Write:[%x]*%x.\n",pD,count);
          
          while (count>0)
          {
            *pD++=*pS++;
            count--;
          }
          break;
          
        default:
          r[2]^=0x80;
          len=1;
          break;
      }
      r[2]^=0x20;
      sck_SendTo(fh0, r, r+2, len);
    }
}





void ConfigTimer1PWM(void)
{
  TCCR1A = 1<<COM1A1 | 1<<COM1B1;
  TCCR1B = 1<<WGM13 | 1<<CS11;
  
  ICR1 = 20000;
  OCR1A = 1500;
  OCR1B = 1500;
  
  PORTB &= ~(1<<1 | 1<<2);
  DDRB |= 1<<1 | 1<<2;
}


uint8_t dhtSckBuf[2+1+2+2];

static void evt2(void)
{
    char len;
        
    len = sck_ReadFrom(fh2, dhtSckBuf, dhtSckBuf+2, sizeof(dhtSckBuf)-2); 
    
    if (len>0)
    {
      switch (dhtSckBuf[2])
      {
        case '?':
        case 'T':
        case 'H':
          dht11_Start(evts_DefToNr(main_tst3));
          break;
                    
        default:
          dhtSckBuf[3]=dhtSckBuf[2];
          dhtSckBuf[4]=len;
          dhtSckBuf[2]=255;
          len=3;
          sck_SendTo(fh2, dhtSckBuf, dhtSckBuf+2, len);
          break;
      }
      sck_SendTo(fh2, dhtSckBuf, dhtSckBuf+2, len);
    }
}


static void tst3(void)
{
  char len=0;
  
  switch (dhtSckBuf[2])
  {
    case '?':
      *((uint16_t*)&dhtSckBuf[3])=2730+(dht11_RecvTemperature()*10);
      *((uint16_t*)&dhtSckBuf[5])=dht11_RecvHumidity()*10;
//      dhtSckBuf[3]=dht11_RecvTemperature();
//      dhtSckBuf[4]=dht11_RecvHumidity();
      len=4;
      break;
    case 'T':
      *((uint16_t*)&dhtSckBuf[3])=2730+(dht11_RecvTemperature()*10);
      len=2;
      break;
    case 'H':
      len=2;
      *((uint16_t*)&dhtSckBuf[3])=dht11_RecvHumidity()*10;
      break;
  }   
  dhtSckBuf[2]^=0x80;
  sck_SendTo(fh2, dhtSckBuf, dhtSckBuf+2, len+1);
}


void main(void) __attribute__((noreturn)) ;

void main(void)
{
  uint8_t cnt0=0;
  uint8_t s[]={'[','@',']','!'};

  evts_init();
  pq_Init();
  Pkt_Init();


  sck_Init();
//  tmr_Init();

  ticp_Init();
  dht11_Init();

//  dht11_Start(evts_DefToNr(main_tst3));


#ifdef __AVR
  sei();
#endif

  fh0 = sck_sck(1);
  sck_bind(fh0,(uint8_t[2]){0,0x03});
  sck_setRecvEvt(fh0, evts_DefToNr(main_evt1));//EVTOFS_MAIN+1);

#if 1

  fh2 = sck_sck(1);
  sck_bind(fh2,(uint8_t[2]){0x21,0x20});
  sck_setRecvEvt(fh2, evts_DefToNr(main_evt2));//EVTOFS_MAIN+2);
#endif


//  dht11_Start(evts_DefToNr(main_tst3));
  
//  tmr_DelayTokenMs(EVTOFS_MAIN+1,1000);

  sck_SendTo(fh0, (uint8_t[2]){0x40,0x40}, "BOOT", 4);
  sck_SendTo(fh0, (uint8_t[2]){0,0x40}, "MAIN", 4);
//  sck_SendTo(fh0, (uint8_t[2]){0,0x40}, "START", 5);

  while (1)
  {
    char stat;
   
    evts_exec(1);
#if 0
    sleep_enable();
    sleep_cpu();
    sleep_disable();
#endif
//    tmr_AddMs(100);
        
    //stat = sck_SendTo(fh0, (uint8_t[2]){0x51,0x52}, s, sizeof(s));

    if (stat>=0)
    {
      s[1]++;
    }
  }
    
}




/*---------------------------------------------------------------------------*/

