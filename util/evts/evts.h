/* Code licensed under GPL3. See license.txt  */



#ifndef _EVTS_H_
#define _EVTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "compdefs.h"
#include "ltenum.h"

typedef void evtsfun_t( void );

typedef evtsfun_t * evtsfuntbl_t;

LTE_REFGROUP(evts);


//extern evtsfuntbl_t * evts_tablelist[];
extern uint8_t evts_irq[];


void evts_init( void );

void evts_post(uint8_t evt);

//void evts_postByDef8(uint8_t nr8);

#define evts_postByDef(e) evts_post(OFS_LOW8(LTE_ENTRYGET(evts, e)))

void evts_exec(uint8_t opt);

void evts_callevt(uint8_t evt);

#define EVTS_DEF_FUNC( name, fn ) LTE_ADDENTRY(evtsfun_t *, evts, name) = fn
#define EVTS_DEF_FUNC_REF(name) &LTE_ENTRYGET(evts, name)

uint8_t evts_DefRefToNr(evtsfun_t ** def);
ALWAYS_INLINE uint8_t evts_DefRefToNr_INLINE(evtsfun_t ** def)
{
    return  (  (uint8_t)def  -  (uint8_t)LTE_GROUPOFS(evts)   ) +1;
}
#define evts_DefToNr(name) OFS_LOW8(LTE_ENTRYGET(evts, name))

ALWAYS_INLINE void evts_postByDefRef(evtsfun_t ** def)
{
    evts_postByDef8((uint8_t)def);
}
//#define evts_postByDef(name) evts_postByDefRef(&LTE_ENTRYGET(evts, name))


ALWAYS_INLINE void evts_irqevt(uint8_t idx, uint8_t evt)
{
    evts_irq[idx]=evt;
}

#ifdef __cplusplus
} //extern C
#endif


#endif //_EVTS_H_

