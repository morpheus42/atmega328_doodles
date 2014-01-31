/*
 */

//#include "config.h"
//#include "board.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include "uart.h"




#ifndef UART0_RXERR
#define UART0_RXERR(p)
#endif

#ifndef UART0_RX
#define UART0_RX(p) (uint8_t)((volatile)UDR0)
#endif

#ifndef UART0_TX
#define UART0_TX(p) 0x55
#endif






ISR(USART_TX_vect)
//ISR(USART_RX_vect)
{
  uint8_t stat;
  
  while ((stat = UCSR0A) & 0x9C)
  {
    if (stat & 0x1C)
    {
      UART0_RXERR(stat);
    }
    if (stat & 0x80)
    {
      UART0_RX(UDR0);
    }
  }

}



ISR(USART_UDRE_vect)
{
  short x;
//  PORTB |= (1<<PB3);  
    x=UART0_TX();
  
    // if there is data then send it
    if (x>=0)
    {
      UDR0 = x;
    } 
    else
    {
       UCSR0B &= ~(1 << UDRIE0); // no more data to send, turn off data ready interrupt
    }
//  PORTB &= ~(1<<PB3);
}


//ISR(xUSART_RX_vect)
//{
//  
//}



