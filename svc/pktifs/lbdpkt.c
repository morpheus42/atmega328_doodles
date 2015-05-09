/* Code licensed under GPL3. See license.txt  */


#include "config.h"
#include "lbdpkt.h"
#include "stdint.h"

#include "evts.h"


static pkt_xferadm_t rxadm;
static pkt_xferadm_t txadm;

static uint8_t myIfId;


static const uint8_t myaddr=0;



static void lbdevt( void )
{

  pktifs_GetRxPkt(myIfId, &rxadm);
  pktifs_GetTxPkt(myIfId,&txadm);
  memcpy(rxadm.buf,txadm.buf,txadm.len);
  rxadm.len = txadm.len;
  pktifs_PktTransmitted(&txadm);
  pktifs_PktReceived(&rxadm);
}




EVTS_DEF_FUNC(lbdpkt_evt,lbdevt);




static pkt_TxPktInQ_ft TxPktInQ;
static void TxPktInQ(void)
{
  evts_post(evts_DefToNr(lbdpkt_evt));
}



void lbdpkt_Init(uint8_t id)
{
  myIfId = id;
}


void SetAdr(uint8_t newadr)
{
  (void) newadr;
}


const pkt_if_t lbdpkt_if=
{
  lbdpkt_Init,
  TxPktInQ,
  SetAdr,
  &myaddr
};



