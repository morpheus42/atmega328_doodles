


#ifndef _UART_C_
#define _UART_C_


#include <stdint.h>
#include "board.h"
#include <avr/io.h>
#include <avr/interrupt.h>


#define SB(p) (1<<p)


extern inline void Uart0_SetBaudrate(uint32_t baud)
{
  UBRR0 = ( (F_CPU/8) / baud)-1;
}

extern inline void Uart0_SetFormat(uint8_t bpc, uint8_t parity, uint8_t stopbits)
{
  uint8_t ucsr0c=0;
  
  if (parity)
  {
    if (parity & 1) ucsr0c |= 0x10;
  }
  
  if (stopbits == 2)
    ucsr0c |= 0x08;
  
  ucsr0c |= ((bpc-1)&0x6);
  
  UCSR0C = ucsr0c;
  
}

extern inline void Uart0_StartTx( void )
{
  UCSR0B |= SB(UDRIE0);
}

extern inline void Uart0_Init( void )
{
  UCSR0A = 0x00;
  UCSR0B = SB(RXCIE0)|SB(RXEN0)|SB(TXEN0);// 0x98;
}


#endif //_UART_C_
