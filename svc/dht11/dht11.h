#ifndef __DHT11_H__
#define __DHT11_H__

#include "stdint.h"
#include "evts.h"

void dht11_Init(void);
void dht11_Start(uint8_t evt);
char dht11_tick(void);

signed char dht11_RecvTemperature(void);

signed char dht11_RecvHumidity(void);
#endif

