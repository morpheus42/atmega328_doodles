/* Code licensed under GPL3. See license.txt  */

#include "stddef.h"
#include "stdint.h"
#include "sck.h"
#include "evts.h"
#include "pkt.h"

#include "config.h"


#ifndef SCK_ADM_NUM
 #define SCK_ADM_NUM (PKT_FIFO_NUM - PKT_FIFO_MIN )
#endif



#define N1_DATAGRAM 1



typedef struct pkthdr_l3_t
{
  uint8_t adrd;
  uint8_t adrs;  
}pkthdr_l3_t;

typedef struct pkthdr_l4_t
{
  uint8_t prtd;
  uint8_t prts;
}pkthdr_l4_t;

typedef struct pkthdr_t
{
  pkthdr_l3_t l3;
  pkthdr_l4_t l4;
}pkthdr_t;



typedef struct sck_adm_t
{
  pq_id_t       fid;      // inPqId
  sck_adr_prt_t adrPrt;   // listening adr+port
//  uint8_t       ofs;      //payload ofs
} sck_adm_t;


#define SCK_ID_IS_VALID(id) (id<SCK_ADM_NUM)


//routing queue
static uint8_t rtQ;
//my address
static uint8_t myadr;


// socket administration
static sck_adm_t adm[SCK_ADM_NUM];



static sck_adm_t * sck_id2adm(sck_id_t id)
{
  return &adm[id];
}


void sck_Init(void)
{
  char i;
  for (i=0;i<SCK_ADM_NUM;i++)
  { // marking all adm as free
    adm[i].fid=PQ_INVALID_PQ_ID;
  }
  // create packetqueue with event
  rtQ=pq_new(EVTOFS_SCK);

  // get all DG packets in rtQ
  Pkt_Subscribe(N1_DATAGRAM,rtQ);
}


sck_id_t sck_sck( uint8_t evt )
{
  char i=SCK_ADM_NUM-1;
  
  while (i>0)
  {
    if (!PQ_ID_IS_VALID(adm[i].fid))
    { // found empty adm
      adm[i].fid=pq_new(0);
      if (PQ_ID_IS_VALID(adm[i].fid))
      {
        pq_SetEvt(adm[i].fid,evt);
        return i;
      }
    }
    i--;
  }
  return i;
}


static uint8_t PrtIsFree( uint8_t prt)
{
  char i=SCK_ADM_NUM-1;
  
  while (i>0)
  {
    if (PQ_ID_IS_VALID(adm[i].fid))
    { // found used adm
      if (adm[i].adrPrt.prt==prt)
      {
        return 0;
      }
    }
    i--;
  }
  return prt;
}

static uint8_t FindFreePrt( uint8_t hint )
{
  uint8_t prt=0x20;
  if ((hint) && (PrtIsFree(hint)))
  {
    return hint;
  }
  
  while (prt>=0x20)
  {
    if (PrtIsFree(prt))
    {
      return prt;
    }
    prt++;
  }
  return 0;
}


int8_t sck_bind(sck_id_t id, sck_adr_prt_t * adrPrt)
{
  sck_adm_t * adm;
  if (SCK_ID_IS_VALID(id))
  {
    adm = sck_id2adm(id);
    adm->adrPrt = *adrPrt;
    adm->adrPrt.prt = FindFreePrt(adm->adrPrt.prt);
    return 0;
  }
  else
  {
    return -1;
  }
}


void sck_setRecvEvt(sck_id_t id, uint8_t evt)
{
  if (SCK_ID_IS_VALID(id))
  {
    pq_SetEvt(sck_id2adm(id)->fid,evt);
  }
}


uint8_t sck_getRecvEvt(sck_id_t id)
{
  if (SCK_ID_IS_VALID(id))
  {
    return pq_GetEvt(sck_id2adm(id)->fid);
  }
  return -1;
}


int8_t sck_SendTo(sck_id_t id, sck_adr_prt_t * adrPrt, uint8_t * data, uint8_t len)
{
  sck_adm_t * adm;

  if (SCK_ID_IS_VALID(id))
  {
    pq_pktid_t pid=Pkt_GetFree();
    if (PKT_ID_IS_VALID(pid))
    {
      sck_adm_t * adm = sck_id2adm(id);
      pkthdr_t * pkta = Pkt_Ptr(pid);


      if (Pkt_SetLen(pid,len + sizeof(pkthdr_t)) >= 0)
      {
        //set src adr:prt
        pkta->l3.adrs = adm->adrPrt.adr;
        if (!pkta->l3.adrs)
        {
          pkta->l3.adrs = myadr;
        }
        pkta->l4.prts = adm->adrPrt.prt;
        //set dest adr:prt
        pkta->l3.adrd = adrPrt->adr;
        pkta->l4.prtd = adrPrt->prt;
              
        memcpy(((uint8_t*)pkta)+sizeof(pkthdr_t), data, len);
      
        // put in routing Q
        pq_Put(rtQ,pid);
            
        return 0;
      }

      Pkt_Free(pid);
    }
  }
  return -1;
}


int8_t sck_ReadFrom(sck_id_t id, sck_adr_prt_t * adrPrt, uint8_t * data, uint8_t maxLen)
{

  if (SCK_ID_IS_VALID(id))
  {
    sck_adm_t * adm = sck_id2adm(id);
    pq_pktid_t pid;
    
    pid = Pkt_Get(adm->fid);
    
    if (PKT_ID_IS_VALID(pid))
    {
      pkthdr_t * pkta = Pkt_Ptr(pid);
      
      if (adrPrt)
      {
        adrPrt->adr = pkta->l3.adrs;
        adrPrt->prt = pkta->l4.prts;
      }
      //TODO: do memcpy
      Pkt_Free(pid);
    }    
    
    return 0;
  }
  return -1;
}

/**************************************************
 *  Routing 
 *
 * note: (parts of)this should possibly move to its own file.. or not..
 **************************************************/

static uint8_t myadr='!'; //TODO: aquire through adr req





static void newpkt(void)
{
  // get pkt id from routing Q
  pq_pktid_t pid = pq_Get( rtQ );
  
  if (PQ_PKT_ID_IS_VALID(pid))
  {
    pkthdr_t * pkta = Pkt_Ptr(pid);
//    printf(":(%x,%x,%x,%x){%s}.\n",pkta->l3.adrd,pkta->l3.adrs,pkta->l4.prtd,pkta->l4.prts,pkta);
  
    // TODO: 1) if me, then for local sockets, else:2
    // TODO: 2) if got route, then use that IF, else:3
    // TODO: 3) use default IF
    
//    if (pkta->l3.adrd = 0x40)
//    {
//    }
//    else
    {
      int8_t ifidx = -1;//TODO: getIFbyDST(pkta->l3.adrd)
      
      if (ifidx<0)
      {
        ifidx=1;
      }
      
      {
        pkthdr_l2_t * pkt2 = (pkthdr_l2_t *)(((uint8_t*)pkta)-sizeof(pkthdr_l2_t));
        pkt2->hwd = 'D'; // TODO: get from found IF[ifidx]
        pkt2->hws = 0; // let lower layer fill in the source hw
        pkt2->typ = N1_DATAGRAM;
        
      }        
      
      pkt_DoQ(pid, ifidx|0x40);
    }
  
  }
  
}







/**************************************************/

const evtsfun_t * sck_evts[SCK_EVT_NUM] =
{
  newpkt,
  NULL,
};





