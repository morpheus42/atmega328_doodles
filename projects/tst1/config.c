
#include "mudem.h"
#include "main.h"
#include "stdint.h"
#include "stddef.h"
#include <avr/io.h>



FUSES = 
{
    .low = 0xFF, //LFUSE_DEFAULT,
    .high = 0xDA, //(FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_EESAVE & FUSE_SPIEN & FUSE_JTAGEN),
    .extended = 0x05, //EFUSE_DEFAULT,
};



mudem_channel_t mudem_channels[8]=
{
  {dbg_Recv,dbg_Txd},
  {main_Recv,main_Txd},
  {NULL,NULL},
  {NULL,NULL},
  {NULL,NULL},
  {NULL,NULL},
  {NULL,NULL},
  {NULL,NULL},
};



/*---------------------------------------------------------------------------*/

