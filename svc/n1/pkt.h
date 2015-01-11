/* Code licensed under GPL3. See license.txt  */

#ifndef __PKT_H__
#define __PKT_H__

#include "stdint.h"
#include "evts.h"
#include "pq.h"

#define PKT_ADR_PTP             0x00
#define PKT_ADR_BCAST_HOP_MASK  0x07
#define PKT_ADR_BCAST_MASKV     0xFE

#define PKT_EVT_NUM 2

#define PKT_FIFO_MIN (3)
// (2+NUM_IFS)


typedef int8_t pktId_t;
//typedef int8_t pktFifoId_t;


typedef struct pkthdr_l2_t
{
  uint8_t hwd;
  uint8_t hws;
  uint8_t typ;
}pkthdr_l2_t;


#define PKT_INVALID_ID (-1)
#define PKT_ID_IS_VALID(id) (id!=PKT_INVALID_ID)


extern const evtsfun_t * pkt_evts[PKT_EVT_NUM];


void Pkt_Init(void);


void Pkt_Subscribe(uint8_t code, pq_id_t pqId);

pq_pktid_t Pkt_GetFree( void );

void Pkt_Free( pq_pktid_t id );

uint8_t * Pkt_Ptr(pq_pktid_t id);

#define Pkt_Get(id) pq_Get(id)

void pkt_DoQ(pq_pktid_t id, int8_t bus);//, uint8_t hwd);

int8_t Pkt_SetLen(pq_pktid_t id, int8_t len);
uint8_t Pkt_GetLen(pq_pktid_t id);

#endif

