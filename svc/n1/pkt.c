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
uint8_t PKT_FIFO_TX;



//#define PKT_ILLEGAL_ID (-1)


//#define TX_Q_PER_BUS





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

// subscribtions:
static pkt_subs_adm_t subsadm[PKT_SUBS_NUM];

// ifs:
static busadm_t busadm[NUM_IFS];




// called by pkt transmitter when pkt has been transmitted
static void cbtx(struct pkt_xferadm_t * _adm)
{
  busxadm_t * adm = (busxadm_t*)_adm;
  pktId_t id;

  printf("txed.\n");
  
  if (adm->xfer.buf)
  {
    id = adm->id;
    
    if ( PKT_ID_IS_VALID(id))
    {
      pq_Put(PKT_FIFO_FREE,id);
#ifndef TX_Q_PER_BUS      
      postevt(EVTOFS_PKT+1);
#endif      
    }
    // Indicate that on return, no new packet for transmit has been set.
    // This will later have to be done by SetTxBuffer()
    adm->xfer.buf=0;
    adm->id = PKT_INVALID_ID;
  }

#ifdef TX_Q_PER_BUS
  id = Pkt_FifoOut(....);

  if PKT_ID_IS_VALID(id)
  {
    pkthdr_t * pkt = pq_Ptr(id);

    adm->id = id;
    adm->xfer.buf = ((char *)pkt)+offsetof(pkthdr_t,hwd);
    adm->xfer.len = buf_Size(id)-offsetof(pkthdr_t,hwd);
  }
#endif
  
}


// called by pkt receiver when new pkt received
static void cbrx(struct pkt_xferadm_t * _adm)
{
  busxadm_t * adm = (busxadm_t*)_adm;
  pktId_t id;

  
  if (adm->xfer.buf)
  {
    pkthdr_t * pkt = (pkthdr_t *)(adm->xfer.buf - offsetof(pkthdr_t,l2));
    id = adm->id;
    
    if ( id >= 0)
    {
      pkt->len = adm->xfer.len;
      pq_Put(PKT_FIFO_RX,id);
    }
    adm->xfer.buf=0;
  }

  
  id = pq_Get(PKT_FIFO_FREE);

  if PQ_PKT_ID_IS_VALID(id)
  {
    pkthdr_t * pkt = pq_Ptr(id);

    pkt->bus = (((char*)adm-(char*)busadm))/sizeof(busadm_t);
    
    adm->id = id;
    adm->xfer.buf = ((char *)pkt)+offsetof(pkthdr_t,l2);
    adm->xfer.len = pq_Size(id)-offsetof(pkthdr_t,l2);
  }
  
}







void Pkt_Init(void)
{
  uint8_t i;

  PKT_FIFO_FREE=pq_new(0);
  PKT_FIFO_RX=pq_new(EVTOFS_PKT);
  PKT_FIFO_TX=pq_new(EVTOFS_PKT+1);
  
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
    if (PQ_PKT_ID_IS_VALID(hb))
    {
      pq_Put(PKT_FIFO_FREE,hb);
    }
  }
  
  // setup all interfaces with receiver buffers
  for (i=0; i<(sizeof(ifstable)/sizeof(ifstable[0])); i++)
  {
    ifstable[i].ifs->Init();
    busadm[i].rx.xfer.cb = cbrx;
    busadm[i].rx.xfer.buf=0;
    cbrx(&busadm[i].rx.xfer);
    ifstable[i].ifs->SetReceiveBuf(&busadm[i].rx.xfer);
    busadm[i].tx.xfer.cb = cbtx;
    busadm[i].tx.id = PKT_INVALID_ID;
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
    pq_Put(PKT_FIFO_TX, id);
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

//  printf("In:%d.%d:(%d)%s.\n",bus,pkta->len,pkta->l2.typ,&pkta->l2);

  if (pkta->len<4)
  {
    pq_Put(PKT_FIFO_FREE,id);
    return;
  }

  if ((hwd == *(ifstable[bus].ifs->hwa))|| (hwd==0) ||(hwd == 0xff))
  { // message was for this device
    uint8_t typ = pkta->l2.typ;
    uint8_t i=PKT_SUBS_NUM;

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
#ifdef TX_Q_PER_BUS      
    uint8_t bus = pkta->bus;
    pq_Put(PKT_FIFO_TX+bus,id);
#else    
    pq_Put(PKT_FIFO_TX,id);
#endif    
  }
    
}




void pktout( void )
{
  pq_pktid_t pid =pq_Peek(PKT_FIFO_TX);
printf("TXO:%d.\n",pid);  
  if (PQ_PKT_ID_IS_VALID(pid))
  {
    pkthdr_t * pkta = pq_Ptr(pid);
    uint8_t bus = pkta->bus;
    busxadm_t * adm = &busadm[bus].tx.xfer;
    
    printf("PktOut(%d->%d):%d:%x.\n",pid,bus,pkta->len,&pkta->l2.hwd);

    if (!PKT_ID_IS_VALID(adm->id))
    {
      adm->id = pid;
      adm->xfer.buf = ((char *)pkta)+offsetof(pkthdr_t,l2);
      adm->xfer.len = pkta->len;
      
      if (ifstable[bus].ifs->SetTransmitBuf(adm)>=0)
      {
        pq_Get(PKT_FIFO_TX);
      }
    }
  }
  else
    printf("Nonextout?.\n");
  
}



const evtsfun_t * pkt_evts[PKT_EVT_NUM] =
{
  newpkt,
  pktout,
};







