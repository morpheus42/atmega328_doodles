/* Code licensed under GPL3. See license.txt  */

#include "stdint.h"
#include "pq.h"
#include "buffl.h"
#include "config.h"
#include "evts.h"



#define PKT_ILLEGAL_ID (-1)


//typedef struct
//{
//  pq_pktid_t b;
//  pq_pktid_t e;
//}fifoPktIds_t;

typedef struct
{
  pq_pktid_t n;
} pkt_LL_t;



typedef struct pq_adm_t
{
  pq_pktid_t    b;
  pq_pktid_t    e;
  uint8_t       evt;
}pq_adm_t;



static pq_adm_t pq_adm[PQ_NUM];


#define PqIdToPq(f) pq_adm[f]

#define PktIdToPkt(i) (*(pkt_LL_t*)buf_Ptr(i))



//======================================================================

void pq_FifoIn(pq_id_t pqId, pq_pktid_t id)
{
  PktIdToPkt(id).n=PKT_ILLEGAL_ID;
  if (!PQ_PKT_ID_IS_VALID(PqIdToPq(pqId).b))
  {
    PqIdToPq(pqId).b = id;
    PqIdToPq(pqId).e = id;
  }
  else
  {
    pq_pktid_t e = PqIdToPq(pqId).e;
    
    PqIdToPq(pqId).e = id;
    PktIdToPkt(e).n=id;
  }
}




pq_pktid_t pq_FifoOut(pq_id_t pqId)
{
  pq_pktid_t ret = PqIdToPq(pqId).b;
  if (PQ_PKT_ID_IS_VALID(ret))
  {
    PqIdToPq(pqId).b = PktIdToPkt(ret).n;
  }
  return ret;
}



//======================================================================

void pq_Init(void)
{
  uint8_t i;
  
  // init fifos adm
  for (i=0;i<PQ_NUM;i++)
  {
    PqIdToPq(i).b=PKT_ILLEGAL_ID;
    PqIdToPq(i).e=PKT_ILLEGAL_ID;
  }

}

// create new packet queue
pq_id_t pq_new( uint8_t recvEvt ) 
{
  uint8_t fid = PQ_NUM-1;
  
  while (fid>0)
  {
    if (PqIdToPq(fid).e == PKT_ILLEGAL_ID)
    { // found free pq
      PqIdToPq(fid).e = 0;
      PqIdToPq(fid).evt = recvEvt;
      return fid;
    }
    fid--;
  }
  return PQ_INVALID_PQ_ID;
}

void pq_SetEvt( pq_id_t h, uint8_t recvEvt)
{
  if (h<PQ_NUM)
  {
    PqIdToPq(h).evt = recvEvt;
  }
}

uint8_t pq_GetEvt( pq_id_t h)
{
  if (h<PQ_NUM)
  {
    return PqIdToPq(h).evt;
  }
  return -1;
}


#if 0
void pq_Free( pq_id_t fid )
{
  pq_pktid_t pid;
  
  while ( (pid = pq_FifoOut(fid))!= 0xff)
  {
    pq_FifoIn(PKT_FIFO_FREE,pid);
  }
  
  PqIdToPq(fid).e=PKT_ILLEGAL_ID;
  
}
#endif


// get packet/buf id from queue h
pq_pktid_t pq_Get( pq_id_t h )
{
  return pq_FifoOut(h);
}

// put packet/buf id into queue h and send event
void pq_Put( pq_id_t h, pq_pktid_t bufid )
{
  pq_FifoIn(h, bufid);
  postevt(pq_adm[h].evt);
}

// get pkt ofs
uint8_t * pq_Ptr( pq_pktid_t bufid)
{
  uint8_t * ptr = buf_Ptr(bufid);
  if (!ptr)
    return 0;
  return ptr+sizeof(pkt_LL_t);
}


// get pkt size
int8_t pq_Size( pq_pktid_t bufid)
{
  return buf_Size(bufid) - sizeof(pkt_LL_t);
}





