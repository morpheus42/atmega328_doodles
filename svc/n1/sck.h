/* Code licensed under GPL3. See license.txt  */

#ifndef __SCK_H__
#define __SCK_H__

#include "stdint.h"
#include "evts.h"



#define SCK_EVT_NUM 2



typedef uint8_t sck_id_t;


typedef struct sck_adr_prt_t
{
  uint8_t adr;
  uint8_t prt;  
}sck_adr_prt_t;



extern const evtsfun_t * sck_evts[SCK_EVT_NUM];



void sck_Init(void);

sck_id_t sck_sck( uint8_t evt );

int8_t sck_bind(sck_id_t id, sck_adr_prt_t * adrPrt);

void sck_setRecvEvt(sck_id_t id, uint8_t evt);

uint8_t sck_getRecvEvt(sck_id_t id);

int8_t sck_SendTo(sck_id_t id, sck_adr_prt_t * adrPrt, uint8_t * data, uint8_t len);

int8_t sck_ReadFrom(sck_id_t id, sck_adr_prt_t * adrPrt, uint8_t * data, uint8_t maxLen);






#endif


