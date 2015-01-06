

#include "config.h"
#include "stddef.h"
#include "board.h"
//#include "circularBuffer.h"
#include "evts.h"
#include "sleep.h"
#include "pkt.h"
#include "sck.h"
#include "pq.h"


static void evt1(void);




const evtsfun_t * main_evts[] =
{
  NULL, // always NULL
  evt1, // evt 1
  NULL, // evt 2
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};



uint8_t bbb[10];


static void evt1(void)
{
  printf("evt1.\n");
}


int main(int argc, char * argv[])
{
  uint8_t cnt0=0;
  sck_id_t fh0;

  evts_init();
  pq_Init();
  Pkt_Init();
  sck_Init();
    
  postevt(EVTOFS_SERPKT);
   
  fh0 = sck_sck(1);
  sck_bind(fh0,(uint8_t[2]){0,0});
  sck_setRecvEvt(fh0, 2);

  while (1)
  {
    uint8_t s[]={'[','@',']','!'};
    uint8_t r[10];
    char stat;
   
    evts_exec(1);

    sleep_enable();
    sleep_cpu();
    sleep_disable();

    postevt(EVTOFS_SERPKT);
    postevt(EVTOFS_SERPKT+1);
    
    
    stat = sck_ReadFrom(fh0, r, r+2, sizeof(r)-2); 
    
    if (stat>0)
    {
      sck_SendTo(fh0, r, r+2, sizeof(r)-2);
    }
    else
    {
      stat = sck_SendTo(fh0, (uint8_t[2]){0x51,0x52}, s, sizeof(s));

      if (stat>=0)
      {
        s[1]++;
      }
    }
  }
  return 0;
}




/*---------------------------------------------------------------------------*/

