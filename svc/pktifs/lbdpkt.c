/* Code licensed under GPL3. See license.txt  */


#include "config.h"
#include "lbdpkt.h"
#include "stdint.h"

#include "evts.h"



static pkt_xferadm_t * rxadm;
static pkt_xferadm_t * txadm;
static const uint8_t myaddr=0;


static void rxchar(void)
{
  if (txadm)
  {  
    // ugly ugly...  refactor to just bounce buffer back
    memcpy(rxadm->buf, txadm->buf, txadm->len);
    rxadm->cb(rxadm);
    postevt(EVTOFS_LBDPKT+1);
  }
}

static void txchar(void)
{
  if (txadm)
  {
    txadm->cb(txadm);
  }
}


const evtsfun_t * lbdpkt_evts[LBDPKT_EVT_NUM] =
{
  rxchar,
  txchar,
};

pkt_SetReceiveBuf_ft lbdpkt_SetReceiveBuf;
int8_t lbdpkt_SetReceiveBuf(pkt_xferadm_t * adm)
{
  if (rxadm)
    return -1;
  rxadm = adm;
  
  return 1;
}

pkt_SetTransmitBuf_ft lbdpkt_SetTransmitBuf;
int8_t lbdpkt_SetTransmitBuf(pkt_xferadm_t * adm)
{
  if (txadm)
    return -1;
  postevt(EVTOFS_LBDPKT);
  return 1;
}


void lbdpkt_Init(void)
{
}


const pkt_if_t lbdpkt_if=
{
  lbdpkt_evts,
  lbdpkt_Init,
  lbdpkt_SetReceiveBuf,
  lbdpkt_SetTransmitBuf,
  &myaddr
};



