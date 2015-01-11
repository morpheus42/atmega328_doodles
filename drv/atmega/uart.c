/*
 */

#include "config.h"
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
#define UART0_TX() 0x55
#endif

#ifndef UART0_TXQNE
#define UART0_TXQNE() 1
#endif

#ifndef UART0_EOFTX()
#define UART0_EOFTX() {}
#endif


ISR(USART_RX_vect)
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


ISR(USART_TX_vect)
{
    UDR0 = 'y';
}


ISR(USART_UDRE_vect)
{
//  PORTB |= (1<<PB3);  
  
    if (UART0_TXQNE())
    { // if there is data then send it
      UDR0 = UART0_TX();
    } 
    else
    {
       UCSR0B &= ~SB(UDRIE0); // no more data to send, turn off data ready interrupt
       UART0_EOFTX();
    }
//  PORTB &= ~(1<<PB3);
}


//ISR(xUSART_RX_vect)
//{
//  
//}



