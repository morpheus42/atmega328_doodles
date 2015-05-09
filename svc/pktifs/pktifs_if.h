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
//  pkt_xferCB_t  * cb;
  uint8_t         pktId;
}pkt_xferadm_t;


typedef void pkt_Init_ft(uint8_t busId);
typedef void pkt_TxPktInQ_ft(void);
typedef void pkt_SetAdr_ft(uint8_t hwAdr);


void pktifs_GetRxPkt(uint8_t ifsId, pkt_xferadm_t * rxadm);
void pktifs_PktReceived(pkt_xferadm_t * rxadm);
void pktifs_GetTxPkt(uint8_t ifsId, pkt_xferadm_t * txadm);
void pktifs_PktTransmitted(pkt_xferadm_t * txadm);



typedef evtsfun_t * evtsfun_fp_t;

//Every pktifs should have this entry:
typedef struct pkt_if_t
{
  pkt_Init_ft           * Init;
  pkt_TxPktInQ_ft       * TxPktInQ;
  pkt_SetAdr_ft         * SetAdr;
  uint8_t               * hwa;
}pkt_if_t;




typedef struct ifentry_const_t
{
  pkt_if_t * ifs;
}ifentry_const_t;

//Table with interfaces. Should be defined by application(config)
extern const ifentry_const_t ifstable[NUM_IFS];




#if 0
//////////////////////
tQ = ifs_init(rQ)

  
  
receive:
  b=getBuf(size) / ifs_setReceiveBuf(b)
  pb=getBufPtr(b)
  
  freeBuf(b)
  tQ.put(b)


transmit:

  ifs_newpkt():
    if (!b)
    {
      b=rQ.get()
    }


  finished():
    freeBuf(b)
    b=rQ.get()
#endif



#endif
