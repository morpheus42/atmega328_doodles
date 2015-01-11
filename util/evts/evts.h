/* Code licensed under GPL3. See license.txt  */



#ifndef _EVTS_H_
#define _EVTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef void evtsfun_t( void );

typedef evtsfun_t * evtsfuntbl_t;



extern evtsfuntbl_t * evts_tablelist[];
extern uint8_t evts_irq[];


void evts_init( void );

void evts_post(uint8_t evt);

void evts_exec(uint8_t opt);

__attribute__ ((always_inline)) extern inline void evts_irqevt(uint8_t idx, uint8_t evt)
{
    evts_irq[idx]=evt;
}

#ifdef __cplusplus
} //extern C
#endif


#endif //_EVTS_H_

