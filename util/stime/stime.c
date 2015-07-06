/* Code licensed under GPL3. See license.txt  */


#include "config.h"
#include "stime.h"
#include "stdint.h"


volatile uint16_t stime_a12;    //  1/65336 of seconds being updated externally.
static uint32_t stime_s;        //seconds
static uint8_t stime_c1;       //  previous 1/256 of seconds.



void timeAddedEvt( void ) // this function should run withing 1/2 a second.
{
  uint8_t tmpA=stime_a12>>8;

  // Note: dt should stay under 0x8000. So updates need to be done within half a second.
  if (stime_c1 & 0x80)
  {
    if (tmpA & 0x80)
    { // got overflow, so add seconds
      stime_s++;
    }
  }
  stime_c1=tmpA;
}


uint16_t stime_16Get_ddSec(void)
{
  uint16_t dd = stime_a12;
  uint16_t ee;

  while ((dd!=(ee=stime_a12))) dd=ee;

  return dd;
}

uint16_t stime_16Get_dSec(void)
{
  return 0;
}

uint16_t stime_16Get_Sec(void)
{
  return (stime_s);
}


void stime_Init(void)
{
}




