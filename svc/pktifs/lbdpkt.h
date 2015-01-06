/* Code licensed under GPL3. See license.txt  */

#ifndef __LBDPKT_H__
#define __LBDPKT_H__

#include "stdint.h"
#include "evts.h"
#include "pktifs_if.h"

#define LBDPKT_EVT_NUM  2


extern const evtsfun_t * lbdpkt_evts[LBDPKT_EVT_NUM];
extern const pkt_if_t lbdpkt_if;


#endif
