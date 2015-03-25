/* Code licensed under GPL3. See license.txt  */

#ifndef __PKTIFS_H__
#define __PKTIFS_H__

#include "stdint.h"
#include "evts.h"
#include "config.h"




struct pkt_xferadm_t;

typedef void pkt_xferCB_t(struct pkt_xferadm_t * adm);

typedef struct pkt_xferadm_t
{
  uint8_t       * buf;
  uint8_t         len;
  pkt_xferCB_t  * cb;
}pkt_xferadm_t;


typedef void pkt_Init_ft(void);
typedef int8_t pkt_SetReceiveBuf_ft(pkt_xferadm_t * adm);
typedef int8_t pkt_SetTransmitBuf_ft(pkt_xferadm_t * adm);


typedef evtsfun_t * evtsfun_fp_t;

typedef struct pkt_if_t
{
//  evtsfun_fp_t          * evts;
  pkt_Init_ft           * Init;
  pkt_SetReceiveBuf_ft  * SetReceiveBuf;
  pkt_SetTransmitBuf_ft * SetTransmitBuf;
  uint8_t               * hwa;
}pkt_if_t;




typedef struct ifentry_const_t
{
  pkt_if_t * ifs;
}ifentry_const_t;

extern const ifentry_const_t ifstable[NUM_IFS];



#endif
