/* Code licensed under GPL3. See license.txt  */

#ifndef __PQ_H__
#define __PQ_H__

#include "stdint.h"
#include "buffl.h"

// pq : PacketQueue

typedef uint8_t pq_id_t;

typedef buf_handle_t pq_pktid_t;


#define PQ_INVALID_PQ_ID       (0xff)

#define PQ_ID_IS_VALID(id) (id!=PQ_INVALID_PQ_ID)



#define PQ_PKT_ID_IS_VALID(id) (id!=-1)


void pq_Init(void);

pq_id_t pq_new( uint8_t recvEvt ); // create new packet queue

void pq_SetEvt( pq_id_t h, uint8_t recvEvt); // set evt

uint8_t pq_GetEvt( pq_id_t h); // get installed evt

pq_pktid_t pq_Get( pq_id_t h ); // get packet/buf id from queue h

void pq_Put( pq_id_t h, pq_pktid_t bufid ); // put packet/buf id into queue h and send event

uint8_t * pq_Ptr( pq_pktid_t bufid); // get pkt ofs

int8_t pq_Len( pq_pktid_t bufid); // get pkt len

int8_t pq_Size( pq_pktid_t bufid); // get pkt size


#endif


