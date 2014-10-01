


#ifndef _UARTC_H_
#define _UARTC_H_


#include <stdint.h>


#define SB(p) (1<<p)


extern inline void Uart0_SetBaudrate(uint32_t baud)
{
  
}

extern inline void Uart0_SetFormat(uint8_t bpc, uint8_t parity, uint8_t stopbits)
{
  
}

void Uart0_StartTx( void );

void Uart0_Init( void );


#endif //_UARTC_H_

