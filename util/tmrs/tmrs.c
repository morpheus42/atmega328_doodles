/* Code licensed under GPL3. See license.txt  */


#include "config.h"
#include "tmrs.h"
#include "stdint.h"


#define STATE_AUTO_RELOAD 0x80
//#define STATE_PAUSED      0x40
#define STATE_MASK        0x80

typedef struct
{
  int8_t next;
  uint8_t token;
  uint16_t delay;
  uint16_t period;
}tmrs_adm_t;

static tmrs_adm_t adm[NUM_TMR];

static int8_t runL=-1;
static int8_t freeL=0;
static int8_t pausedL=-1;


void tmrs_dmp(void)
{
  char i;
  
  printf("%d,%d,%d.\n",freeL,runL,pausedL);
  for (i=0;i<NUM_TMR;i++)
  {
    printf("%x:%d,%d,%x\n",i,adm[i].delay,adm[i].next,adm[i].token);
  }
}



void tmrs_Init(void)
{
  int8_t idx=NUM_TMR-1;

  do
  {
    idx--;
    adm[idx].next=idx+1;
  }
  while (idx);
  adm[NUM_TMR-1].next=-1;
}


static int8_t getFreeSlot(void)
{
  int8_t idx=freeL;

  if (idx>=0)
  {
    freeL=adm[idx].next;
  }
  return idx;
}


int8_t RemoveFromList(int8_t * listFirst, int8_t id)
{
  int8_t idx = *listFirst;
  
  if (idx == id)
  {
    *listFirst = adm[idx].next;
    return 1;
  }
  
  while (idx>=0)
  {
    if (adm[idx].next == id)
    {
      adm[idx].next = adm[id].next;
      return 1;
    }
  }
  return 0;
}


void InsertByTime(int8_t slot)
{
  int8_t idx=runL;
  int8_t last=-1;
  uint16_t delayMs = adm[slot].delay;

  while (idx>=0)
  {
    if (delayMs > adm[idx].delay)
    {
      delayMs -= adm[idx].delay;
      last = idx;
      idx = adm[idx].next;
    }
    else
    {
      break;
    }
  }
  
  adm[slot].delay=delayMs;
  adm[slot].next=idx;
  
  if (idx>=0)
  {
    adm[idx].delay-=delayMs;
  }
  if (last>=0)
  {
    adm[last].next=slot;
  }
  else
  {
    runL=slot;
  }
}


uint8_t tmrs_GetTimer(uint8_t token, uint16_t period)
{
    int8_t idx = getFreeSlot();
    if (idx>=0)
    {
      adm[idx].token = token;
      adm[idx].period = period;
    }
    return idx;
}


void tmrs_Stop(uint8_t tmrId)
{
  if (!RemoveFromList(&runL,tmrId))
  {
      return;
  }
  adm[tmrId].next = pausedL;
  pausedL=tmrId;
}


void tmrs_Start(uint8_t tmrId)
{
  if (!RemoveFromList(&pausedL,tmrId))
  {
    if (!RemoveFromList(&runL,tmrId))
    {
      return;
    }
  }
  
  InsertByTime(tmrId);
}


void tmrs_SetToken(uint8_t tmrId,uint8_t token)
{
  adm[tmrId].token = token;
}


void tmrs_SetPeriod(uint8_t tmrId, uint16_t period)
{
  adm[tmrId].period = period;
}


void tmrs_SetOneshot(uint8_t tmrId)
{
  adm[tmrId].period=0;
}


void tmrs_Free(uint8_t tmrId)
{  
  if (!RemoveFromList(&pausedL,tmrId))
  {
    if (!RemoveFromList(&runL,tmrId))
    {
      return;
    }
  }
    
  adm[tmrId].next=freeL;
  freeL=tmrId;
}



uint8_t tmrs_UnregOneShot(uint8_t token, uint16_t delayMs)
{
  int8_t slot = tmrs_GetTimer(token,delayMs);
  int8_t idx=runL;
  int8_t last=-1;
  
  if (slot<0)
    return;

  tmrs_SetOneshot(slot);
  tmrs_Start(slot);
  
  return slot;  
}



uint16_t tmr_GetNextExp(void)
{
  if (runL<0)
    return 60000;
  return adm[runL].delay;
}



void tmrs_AddMs(uint16_t delta)
{
  int8_t idx;
  
  while ((delta)&&(runL>=0))
  { 
    uint16_t delay = adm[runL].delay;
    
    if (!delay)
    {
      break;
    }
    
    if (delta<delay)
    {
      adm[runL].delay=delay-delta;
      break;
    }
    delta-=delay;
    TMR_TOKEN_EXPIRED(adm[runL].token);

    idx=runL;
    runL=adm[runL].next;  

    adm[idx].next=freeL;
    freeL=idx;
  }
}





