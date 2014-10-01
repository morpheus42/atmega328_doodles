/*
 */

#include "config.h"
//#include "board.h"
#include "stddef.h"
#include "uartc.h"
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include <poll.h>     



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




#if 0
ISR(USART_RX_vect)
{
  uint8_t stat;
  
  while ((stat = 0) & 0x9C)
  {
    if (stat & 0x1C)
    {
      UART0_RXERR(stat);
    }
    if (stat & 0x80)
    {
     // UART0_RX(UDR0);
    }
  }

}
#endif

#if 0
ISR(USART_TX_vect)
{
    UDR0 = 'y';
}
#endif

#if 0
ISR(USART_UDRE_vect)
{
//  PORTB |= (1<<PB3);  
  
    if (UART0_TXQNE())
    { // if there is data then send it
     // UDR0 = UART0_TX();
    } 
    else
    {
     //  UCSR0B &= ~SB(UDRIE0); // no more data to send, turn off data ready interrupt
    }
//  PORTB &= ~(1<<PB3);
}
#endif

//ISR(xUSART_RX_vect)
//{
//  
//}



static struct pollfd fds[]=
{
  { 0, POLLIN, 0},
  { 1, POLLOUT*0, 0},
};

static void * run(void *arg)
{
  int res=0;
  int len;
  char buf[10];
  
  while (1)
  {  
    res = poll(&fds,2, 100);

    if (fds[0].revents)
    {
      len = read(0,&buf[0],1);
      if (len>0)
      {
        UART0_RX(buf[0]); 
      }
    }
    if (fds[1].revents)
    {
      if (UART0_TXQNE())
      { // if there is data then send it
         buf[0] = UART0_TX();
         write(1,buf,1);
      } 
      else
      {
        fds[1].events=0; // no more data to send, turn off data ready interrupt
      }
    }
    
  }
  
  return NULL;
}


void Uart0_StartTx( void )
{
  fds[1].events=POLLOUT;
}



static pthread_t thread;

void Uart0_Init( void )
{
  int s;
 
  s = pthread_create(&thread, NULL, &run, NULL);
  
}

