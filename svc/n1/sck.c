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
  pq_id_t       qid;      // inPqId
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
    adm[i].qid=PQ_INVALID_PQ_ID;
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
    if (!PQ_ID_IS_VALID(adm[i].qid))
    { // found empty adm
      adm[i].qid=pq_new(0);
      if (PQ_ID_IS_VALID(adm[i].qid))
      {
        pq_SetEvt(adm[i].qid,evt);
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

  while (i>=0)
  {
    if (PQ_ID_IS_VALID(adm[i].qid))
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
  uint8_t prt=adrPrt->prt;
  
  if (SCK_ID_IS_VALID(id))
  {
    adm = sck_id2adm(id);
    prt = FindFreePrt(prt);
    adm->adrPrt.adr = adrPrt->adr;
    adm->adrPrt.prt = prt;
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
    pq_SetEvt(sck_id2adm(id)->qid,evt);
  }
}


uint8_t sck_getRecvEvt(sck_id_t id)
{
  if (SCK_ID_IS_VALID(id))
  {
    return pq_GetEvt(sck_id2adm(id)->qid);
  }
  return -1;
}


int8_t sck_SendTo(sck_id_t id, sck_adr_prt_t * adrPrt, uint8_t * data, uint8_t len)
{
  sck_adm_t * adm;

  if (SCK_ID_IS_VALID(id))
  {
    pq_pktid_t pid=Pkt_GetFree();
printf("STF:%d.\n",pid);
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
    
    pid = Pkt_Get(adm->qid);

    if (PKT_ID_IS_VALID(pid))
    {
      pkthdr_t * pkta = Pkt_Ptr(pid);
      uint8_t len = Pkt_GetLen(pid)-sizeof(pkthdr_t);
            
      if (adrPrt)
      {
        adrPrt->adr = pkta->l3.adrs;
        adrPrt->prt = pkta->l4.prts;
      }

      memcpy(data, ((uint8_t*)pkta)+sizeof(pkthdr_t), len);

      Pkt_Free(pid);
      return len;
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
    printf(":(%x.%x->%x.%x){%s}.\n",pkta->l3.adrs,pkta->l4.prts,pkta->l3.adrd,pkta->l4.prtd,pkta);
  
    // TODO: 1) if me, then for local sockets, else:2
    // TODO: 2) if got route, then use that IF, else:3
    // TODO: 3) use default IF
    
    if ((pkta->l3.adrd == myadr)||(pkta->l3.adrd==0)||(pkta->l3.adrd==0xff))
    {
      char i=SCK_ADM_NUM-1;
      
      while (i>=0)
      { // search for matching sck (no need to check adr because we currently have only one per system)
        if (PQ_ID_IS_VALID(adm[i].qid)&&(pkta->l4.prtd == adm[i].adrPrt.prt))
        {
          pq_Put(adm[i].qid,pid);
          break;
        }
        i--;
      }
      if ((i<0)&&(pkta->l3.adrd == myadr))
      { // no matching socket, free packet
        Pkt_Free(pid);
      }
    }
    
    if (pkta->l3.adrd != myadr)
    {
      int8_t ifidx = -1;//TODO: getIFbyDST(pkta->l3.adrd)
      int8_t ifcnt = 1;
      //NUM_IFS
      
      if (ifidx<0)
      {
        ifidx=1; //TODO: select default IF
      }
      
      while (ifcnt>0)
      {
        pq_pktid_t pidt=pid;
        ifcnt--;
        if (ifcnt)
        {
          pidt=Pkt_Dup(pid);
        }
        
        if (PQ_PKT_ID_IS_VALID(pidt))
        {
          pkthdr_l2_t * pkt2 = (pkthdr_l2_t *)(((uint8_t*)Pkt_Ptr(pidt))-sizeof(pkthdr_l2_t));
          pkt2->hwd = 0; // TODO: get from found IF[ifidx]
          pkt2->hws = 0; // let lower layer fill in the source hw
          pkt2->typ = N1_DATAGRAM;
          pkt_DoQ(pidt, ifidx|0x40);
        }
        ifidx++;
        
      }
      
    }
  
  }
  
}




/*

hw adr resolution

  |dst=00|src|typ=00|cmd|XXX
  
    cmd='Q': Who has adr?
      XXX:|adr|uidlen|uidtype|uid|
    cmd='N': I got (response to 41)
      XXX:|adr|uidlen|uidtype|uid|
    cmd='U': I take
      XXX:|adr|uidlen|uidtype|uid|
    cmd='M': Move to
      XXX:|adr|to|

States:
    
   OnMsg:
   'Q': If me==adr: Reply with 'N'
   'U': If me==src: me=00,NextAdr,UseAdr,SendQ
   'N': If me==src, then me=00,NextAdr,UseAdr,SendQ
   'M': If me==To, 



*/


/**************************************************/

const evtsfun_t * sck_evts[SCK_EVT_NUM] =
{
  newpkt,
  NULL,
};





