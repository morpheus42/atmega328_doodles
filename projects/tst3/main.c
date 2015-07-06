

#include "config.h"
#include "stddef.h"
#include "board.h"
#include "ltenum.h"
#include "evts.h"
#include "sleep.h"
#include "pkt.h"
#include "sck.h"
#include "pq.h"
#include "tmrs.h"

#ifdef __AVR
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

static void evt1(void);

EVTS_DEF_FUNC(main_evt1,evt1);


#ifndef __AVR
uint16_t OCR1A;
uint16_t OCR1B;
uint16_t ICR1;
uint8_t  PORTD;
#endif

void ConfigTimer1PWM(void)
{
#ifdef __AVR
  TCCR1A = 1<<COM1A1 | 1<<COM1B1;
  TCCR1B = 1<<WGM13 | 1<<CS11;
#endif  

  ICR1 = 20000;
  OCR1A = 1500;
  OCR1B = 1500;

#ifdef __AVR
  PORTB &= ~(1<<1 | 1<<2);
  DDRB |= 1<<1 | 1<<2;
#endif
}

 

uint8_t bbb[10];

static sck_id_t fh0;


static void evt1(void)
{
    char stat;
    uint8_t r[6];
    
    printf("evt1.\n");
    
    stat = sck_ReadFrom(fh0, r, r+2, sizeof(r)-2); 
    
    if (stat>0)
    {
      switch (r[2])
      {
        case 'L':
          //ledpower:
          // r[3];
          printf("LedOutput:%d.\n",r[3]);
          if (r[3]&1)
          {
            PORTD |= (1<<5);
          }
          else
          {
            PORTD &= ~(1<<5);
          }

          r[2]^=0x20;
          break;
          
        case 'P':
          // position
          OCR1A = *(uint16_t*)&r[3];
          OCR1B = *(uint16_t*)&r[5];
          printf("pwmA,pwmB:%d,%d.\n",OCR1A,OCR1B);
          r[2]^=0x20;
          break;
          
        default:
          r[2]^=0x80;
          break;
      }
      sck_SendTo(fh0, r, r+2, 1);
    }
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
  tmrs_Init();


#ifdef __AVR
  DDRD |= (1<<5);
  PORTD &= ~(1<<5);
#endif

#ifdef __AVR
  sei();
#endif
  
  ConfigTimer1PWM();

  fh0 = sck_sck(1);
  sck_bind(fh0,(uint8_t[2]){0,0x45});
  PORTD=0;
  sck_setRecvEvt(fh0, evts_DefToNr(main_evt1));//EVTOFS_MAIN+1);
  PORTD=0xff;
  
//  tmr_DelayTokenMs(EVTOFS_MAIN+1,1000);

  sck_SendTo(fh0, (uint8_t[2]){0,0x44}, "BOOT", 4);
  sck_SendTo(fh0, (uint8_t[2]){0,0x44}, "MAIN", 4);
  sck_SendTo(fh0, (uint8_t[2]){0,0x44}, "START", 5);

  while (1)
  {
    char stat;
   
    evts_exec(1);
#if 1
    sleep_enable();
    sleep_cpu();
    sleep_disable();
#endif
    tmrs_AddMs(100);
        
//    stat = sck_SendTo(fh0, (uint8_t[2]){0x51,0x52}, s, sizeof(s));

    if (stat>=0)
    {
      s[1]++;
    }
  }
    
}




/*---------------------------------------------------------------------------*/

