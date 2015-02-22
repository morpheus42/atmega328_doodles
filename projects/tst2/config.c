#include "stdint.h"
#include "stddef.h"
//#include <avr/io.h>

#include "pkt.h"
#include "sck.h"
#include "serpkt.h"
#include "lbdpkt.h"
#include "main.h"
#include "dht11.h"


#if 0
FUSES = 
{
    .low = 0xFF, //LFUSE_DEFAULT,
    .high = 0xDA, //(FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_EESAVE & FUSE_SPIEN & FUSE_JTAGEN),
    .extended = 0x05, //EFUSE_DEFAULT,
};
#endif


// ifs table:
const ifentry_const_t ifstable[NUM_IFS]=
{
  {&lbdpkt_if},
  {&serpkt_if}  
};




evtsfuntbl_t * evts_tablelist[]=EVTS_TABLELIST;





/*---------------------------------------------------------------------------*/

