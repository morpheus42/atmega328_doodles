/* Code licensed under GPL3. See license.txt  */

#ifndef __SERPKT_H__
#define __SERPKT_H__

#include "stdint.h"
#include "evts.h"
#include "pktifs_if.h"

#define SERPKT_EVT_NUM  2


const evtsfun_t * serpkt_evts[SERPKT_EVT_NUM];
extern const pkt_if_t serpkt_if;


#endif
