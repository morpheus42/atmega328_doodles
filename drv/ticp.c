

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ticp.h"


static char bothEdges=0;
static void * buf;

void ticp_config(void * nbuf, char cfg)
{
  TCCR1B=0;
  TCCR1A=0;
    
  //Set Initial Timer value
  TCNT1=0;
  //clear interrupt flags to avoid any pending interrupts
  TIFR1=(1<<ICF1)|(1<<TOV1);

  bothEdges = ((cfg & TICP_EDGE_BOTH ) !=0);

  //Enable input capture and overflow interrupts
  TIMSK1|=(1<<ICIE1)|(1<<TOIE1);
  
  //First capture on rising edge would be TCCR1B|=(1<<ICES1);
  //First capture on falling edge would be TCCR1B&=~(1<<ICES1);
  // Set edge capture direction and clock divider:
  TCCR1B = (TCCR1B & 0xB8) | (cfg & ~TICP_EDGE_BOTH) | 0x80;

  buf=nbuf;
//  sei();
  DDRB |= (1<<PB1);
}



ISR(TIMER1_CAPT_vect)
{
  unsigned char dat;
  
//  PORTB |= (1<<PB5);

  // Read capture-register low byte  
  dat = ICR1L;
  
  // check capture-regiser high byte
  if (ICR1H>0)
  { // high byte not zero, so out of range
    dat=255;
    // reset timer
    TCNT1=4;
  }
  else
  { // high byte zero, dat in range
    // Substract captured time. What is left is for next edge.
    TCNT1-=dat;
  }

  
  if (bothEdges)
  { // we want to trigger on both edges, so flip trigger direction
    TCCR1B^=(1<<ICES1);
  }


  CircularBufWrite_INLINE(buf,dat);  // Using inline because a call will push all registers on the stack when the compiler knows it is in interrupt.
//  PORTB &= ~(1<<PB5);

}


ISR(TIMER1_OVF_vect)
{
//  PORTB |= (1<<PB1);
  //increment overflow counter
  //T1Ovs2++;
//  PORTB &= ~(1<<PB1);
}





