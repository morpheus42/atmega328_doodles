#include "stddef.h"
#include "board.h"
#include "uart.h"
#include "circularBuffer.h"
//#include "mudem.h"
//#include "main.h"
//#include "ltenum.h"
#include "evts.h"


uint8_t Q_evt[M_CircularBufferCalcRequiredMemorySizeForBuffer(10)];


uint8_t uart0_in[M_CircularBufferCalcRequiredMemorySizeForBuffer(5)];
uint8_t uart0_out[M_CircularBufferCalcRequiredMemorySizeForBuffer(5)];



static void echo(void)
{
  while (CircularBufNotEmpty(uart0_in))
  {
    uint8_t c=CircularBufRead(uart0_in);
    
    c ^= 0x20;
    
    CircularBufWrite(uart0_out,c);
    Uart0_StartTx();
  }

  CircularBufWrite(Q_evt,1);
}






static const evtsfun_t * myevts[] =
{
  NULL,
  echo,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};





int main(int argc, char * argv[])
{


  CircularBufInit(Q_evt,sizeof(Q_evt));
  CircularBufInit(uart0_in,sizeof(uart0_in));
  CircularBufInit(uart0_out,sizeof(uart0_out));

  Uart0_Init();
//  Uart0_SetBaudrate(115200);
  Uart0_SetBaudrate(19200);
  Uart0_SetFormat(8, 1, 2);

  echo();
  
  while (1)
  {
    uint8_t evt;  // 0 is not valid evt. 0 _can_ mean empty buffer
    while ((evt=CircularBufRead(Q_evt)))
    {
      evtsfun_t ** EvtFunPtrTbl=0;
      
      switch (evt & 0xF8)
      {
        case 0x00:
          EvtFunPtrTbl = myevts;
          break;
      
        case 0x10:
          EvtFunPtrTbl = 0;
          break;
          
      }
      if (EvtFunPtrTbl)
      {
        evtsfun_t * FunPtr=EvtFunPtrTbl[evt&0xf];
        if (FunPtr)
          FunPtr();
      }
    }
  }
  return 0;
}




/*---------------------------------------------------------------------------*/

