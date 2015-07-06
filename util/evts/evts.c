/* Code licensed under GPL3. See license.txt  */


#include "config.h"
#include "evts.h"
#include "stddef.h"
#include "stdint.h"
#include "ltenum.h"

//#include "uart.h"


static uint8_t Q_evt[M_CircularBufferCalcRequiredMemorySizeForBuffer(EVTS_QSIZE)];

#ifdef EVTS_IRQSIZE
uint8_t evts_irq[EVTS_IRQSIZE];
#endif



void evts_init( void )
{
  CircularBufInit(Q_evt,sizeof(Q_evt));
}

void evts_post(uint8_t evt)
{
  uint8_t base = (uint8_t)LTE_GROUPOFS(evts);
  evt-=base;
//  printf("(%x).\n",evt);
  CircularBufWrite(Q_evt, evt+1);
}


static uint8_t getEvt( void )
{
#ifdef EVTS_IRQSIZE
  uint8_t i=0;
  
  while (i<EVTS_IRQSIZE)
  {
    uint8_t evt = evts_irq[i];
    
    if (evt)
    {
      uint8_t base = (uint8_t)LTE_GROUPOFS(evts);
      evts_irq[i]=0;
      return evt-base+1;
    }
    i++;
  }
#endif
  return CircularBufRead(Q_evt);
}

void evts_callevt(uint8_t evt)
{
    evtsfun_t * FunPtr;

//    Uart0_Dbg_Outc(0);
//    Uart0_Dbg_Outc(evt);
//    Uart0_Dbg_OutHexW(EvtFunPtrTbl);
    
    FunPtr = (evtsfun_t *) LTE_ReadU16FromGroup(evts, evt);
    
//    printf("%x():%x,%x.\n",evt,EvtFunPtrTbl,FunPtr);

//    Uart0_Dbg_OutHexW(FunPtr);
    
    if (FunPtr)
    {
      FunPtr();
    }
}



void evts_exec(uint8_t opt)
{
  uint8_t evt;  // 0 is not valid evt. 0 _can_ mean empty buffer

  while ((evt=getEvt()))
  {
    evt--;
    evts_callevt(evt);

    if ((opt&1)==0)
      break;
  }

}

uint8_t evts_DefRefToNr(evtsfun_t ** def)
{
  return evts_DefRefToNr_INLINE(def);
}




