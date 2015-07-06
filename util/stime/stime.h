/* Code licensed under GPL3. See license.txt  */



#ifndef _STIME_H_
#define _STIME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"


#ifndef STIME_IRQ()
#define STIME_IRQ() {}
#endif

extern volatile uint16_t stime_a12;


void stime_Init(void);

__attribute__ ((always_inline)) inline stime_Add_dd8(uint8_t dt)
{
  stime_a12 += dt;
  STIME_IRQ();
}

__attribute__ ((always_inline)) inline stime_Add_dd16(uint16_t dt)
{
  stime_a12 += dt;
  STIME_IRQ();
}


uint16_t stime_16Get_uSec(void);
uint16_t stime_16Get_mSec(void);
uint16_t stime_16Get_Sec(void);


#ifdef __cplusplus
} //extern C
#endif


#endif //_STIME_H_

