/* Code licensed under GPL3. See license.txt  */



#ifndef _TMR_H_
#define _TMR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"


void tmrs_Init(void);

void tmrs_AddMs(uint16_t delta);

uint16_t tmrs_GetNextExp(void);

void tmrs_dmp(void);


uint8_t tmrs_GetTimer(uint8_t token, uint16_t period);
void tmrs_Stop(uint8_t tmrId);
void tmrs_Start(uint8_t tmrId);
void tmrs_SetToken(uint8_t tmrId,uint8_t token);
void tmrs_SetPeriod(uint8_t tmrId, uint16_t period);
void tmrs_OneShot(uint8_t tmrId);
void tmrs_Free(uint8_t tmrId);
uint8_t tmrs_UnregOneShot(uint8_t token, uint16_t delay);



#ifdef __cplusplus
} //extern C
#endif


#endif //_TMR_H_

