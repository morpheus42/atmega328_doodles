/* Code licensed under GPL3. See license.txt  */


#include "config.h"
#include "evts.h"
#include "stdint.h"

#ifndef EVTS_EVTSHIFT
 #error EVTS_EVTSHIFT not defined !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif

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
  CircularBufWrite(Q_evt,evt);
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
      evts_irq[i]=0;
      return evt;
    }
    i++;
  }
#endif
  return CircularBufRead(Q_evt);
}


void evts_exec(uint8_t opt)
{
  uint8_t evt;  // 0 is not valid evt. 0 _can_ mean empty buffer

  while ((evt=getEvt()))
  {
    evtsfun_t ** EvtFunPtrTbl = evts_tablelist[evt>>EVTS_EVTSHIFT];
    
    evtsfun_t * FunPtr = EvtFunPtrTbl[evt&(0xFF>>(8-EVTS_EVTSHIFT))];

    if (FunPtr)
      FunPtr();

    if ((opt&1)==0)
      break;
  }

}



