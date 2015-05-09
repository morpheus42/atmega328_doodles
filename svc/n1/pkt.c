/* Code licensed under GPL3. See license.txt  */

#include "config.h"
#include "pkt.h"
//#include "buffl.h"
#include "pktifs_if.h"
#include "evts.h"
#include "stddef.h"
#include "pq.h"

#define PKT_NUM 5
#define PKT_SIZE 20

#define PKT_SUBS_NUM 3

uint8_t PKT_FIFO_FREE;
uint8_t PKT_FIFO_RX;
uint8_t busTxQ[NUM_IFS];






typedef struct pkthdr_t
{
  uint8_t bus;
  uint8_t len;
  pkthdr_l2_t l2;
}pkthdr_t;



typedef struct busxadm_t
{
  pkt_xferadm_t xfer;
  pktId_t id;
}busxadm_t;

typedef struct busadm_t
{
  busxadm_t rx;
  busxadm_t tx;
}busadm_t;


typedef struct pkt_subs_adm_t
{
  uint8_t typ;
  pq_id_t pqId;
}pkt_subs_adm_t;



static void newpkt( void );
EVTS_DEF_FUNC(pkt_newpkt, newpkt);



// subscribtions:
static pkt_subs_adm_t subsadm[PKT_SUBS_NUM];


void pktifs_GetRxPkt(uint8_t ifsId, pkt_xferadm_t * rxadm)
{
  pktId_t id;
  
  id = pq_Get(PKT_FIFO_FREE);

  if PQ_PKT_ID_IS_VALID(id)
  {
    pkthdr_t * pkt = pq_Ptr(id);

    pkt->bus = ifsId;
    
    rxadm->pktId = id;
    rxadm->buf = ((char *)pkt)+offsetof(pkthdr_t,l2);
    rxadm->len = pq_Size(id)-offsetof(pkthdr_t,l2);
  }
}

void pktifs_PktReceived(pkt_xferadm_t * rxadm)
{
  pkthdr_t * pkt = pq_Ptr(rxadm->pktId);

  pkt->len=rxadm->len;
  rxadm->len=0;
  
  pq_Put(PKT_FIFO_RX,rxadm->pktId);  
}



void pktifs_GetTxPkt(uint8_t ifsId, pkt_xferadm_t * txadm)
{
  pktId_t id;
  
  id = pq_Get(busTxQ[ifsId]);

  if PQ_PKT_ID_IS_VALID(id)
  {
    pkthdr_t * pkt = pq_Ptr(id);
    
    txadm->pktId = id;
    txadm->buf = ((char *)pkt)+offsetof(pkthdr_t,l2);
    txadm->len = pq_Size(id)-offsetof(pkthdr_t,l2);
  }
  else
  {
    txadm->buf=0;
  }
}




void pktifs_PktTransmitted(pkt_xferadm_t * txadm)
{
  pq_Put(PKT_FIFO_FREE,txadm->pktId);
  txadm->buf = 0;
}





void Pkt_Init(void)
{
  uint8_t i;

  PKT_FIFO_FREE=pq_new(0);
  PKT_FIFO_RX=pq_new(evts_DefToNr(pkt_newpkt));
  
  //initialize protocol subscribe list
  for (i=0;i<PKT_SUBS_NUM;i++)
  {
    subsadm[i].pqId=-1;
  }
  
  // allocate pkt buffers
  for (i=0;i<PKT_NUM;i++)
  {
    buf_handle_t hb = buf_Get(12);
//    printf("%x:%x.\n",hb,PktIdToPkt(hb));
    {
      pq_Put(PKT_FIFO_FREE,hb);
    }
  }
  
  // setup all interfaces with receiver buffers
  for (i=0; i<(sizeof(ifstable)/sizeof(ifstable[0])); i++)
  {
    if (ifstable[i].ifs)
    {
      ifstable[i].ifs->Init(i);
    }
    busTxQ[i]=pq_new(0);
  }
  
}

void Pkt_Subscribe(uint8_t code, pq_id_t pqId)
{
  uint8_t i=PKT_SUBS_NUM;
  
  while (i>0)
  {
    i--;
    if (!PQ_ID_IS_VALID(subsadm[i].pqId))
    {
      subsadm[i].typ=code;
      subsadm[i].pqId=pqId;
      break;
    }
  }
}




//Add pkt to rx/tx queue
void pkt_DoQ(pq_pktid_t id, int8_t bus)//, uint8_t hwd)
{
  pkthdr_t * pkta = pq_Ptr(id);
  if (bus>=0)
  {
    pkta->bus = bus & 0x3f;
  }
  
  if (!pkta->l2.hws)
  {
    pkta->l2.hws = *(ifstable[pkta->bus].ifs->hwa);
  }

//  printf("PktDoQ(%d->%d):%d:%s.\n",id,pkta->bus,pkta->len,&pkta->l2.hwd);
  
  if (bus & 0x40)
  {
    bus&=0x3f;
    pq_Put(busTxQ[bus],id);
    ifstable[bus].ifs->TxPktInQ();
  }
  else
  {
    pq_Put(PKT_FIFO_RX, id);
  }
}


pq_pktid_t Pkt_GetFree( void )
{
  return pq_Get(PKT_FIFO_FREE);
}

pq_pktid_t Pkt_Dup(pq_pktid_t pid)
{
  pq_pktid_t dup = Pkt_GetFree();
  
  if (PQ_PKT_ID_IS_VALID(dup))
  {
    pkthdr_t * pkta = pq_Ptr(pid);
    
    memcpy(pq_Ptr(dup),pq_Ptr(pid),pkta->len);
  }
  return dup;
}


void Pkt_Free( pq_pktid_t pktid)
{
  pq_Put(PKT_FIFO_FREE,pktid);
}

uint8_t * Pkt_Ptr(pq_pktid_t pktid)
{
  return pq_Ptr(pktid)+sizeof(pkthdr_t);
}



int8_t Pkt_SetLen(pq_pktid_t id, int8_t len)
{
  pkthdr_t * pkta = pq_Ptr(id);
  
  len+=sizeof(pkthdr_l2_t);
  
  if ( len < buf_Size(id))
  {
    pkta->len = len;
  }
  return buf_Size(id)-len;
}

uint8_t Pkt_GetLen(pq_pktid_t id)
{
  pkthdr_t * pkta = pq_Ptr(id);
  return pkta->len-sizeof(pkthdr_l2_t);
}




//Handle new pkt in incomming queue/fifo
void newpkt( void )
{
  pq_pktid_t id =pq_Get(PKT_FIFO_RX);
  pkthdr_t * pkta = pq_Ptr(id);
  uint8_t bus = pkta->bus;
  uint8_t hwd = pkta->l2.hwd;
  uint8_t bushwd = *(ifstable[bus].ifs->hwa);

//  printf("In:%d.%d:(%d)%s.\n",bus,pkta->len,pkta->l2.typ,&pkta->l2);

  if (pkta->len<4)
  {
    pq_Put(PKT_FIFO_FREE,id);
    return;
  }

  if ((hwd == bushwd)|| (hwd==0) ||(hwd == 0xff)||(bushwd==0))
  { // message was for this device
    uint8_t typ = pkta->l2.typ;
    uint8_t i=PKT_SUBS_NUM;
    typ=1;

    while (i>0)
    {
      i--;

      if (subsadm[i].typ == typ)
      {
        pq_Put(subsadm[i].pqId,id);
        break;
      }
    }
    
  }
  else
  {
    //forward
    uint8_t bus = pkta->bus & 0x3f;
    pq_Put(busTxQ[bus],id);
    ifstable[bus].ifs->TxPktInQ();
  }
    
}











