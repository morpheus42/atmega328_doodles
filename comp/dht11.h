


#ifndef __DHT11_H__
#define __DHT11_H__


void dht11_init(void);
void dht11_start(void);
char dht11_tick(void);

signed char dht11_latest_temperature(void);

signed char dht11_latest_humidity(void);




#endif

