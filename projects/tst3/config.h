#ifndef _CONFIG_C_
#define _CONFIG_C_

#include "stdint.h"
#include "circularBuffer.h"
#include "evts.h"


#ifdef NOPRINTF
#define printf(...) {}
#endif



extern uint8_t uart0_out[];
extern uint8_t uart0_in[];
#define UART0_RX(p)   {CircularBufWrite_INLINE(uart0_in,p);evts_irqevt(EVTS_IRQ_UART0RX,EVTOFS_SERPKT);}
#define UART0_TX()    CircularBufRead_INLINE(uart0_out)
#define UART0_TXQNE() CircularBufNotEmpty_INLINE(uart0_out)
#define UART0_EOFTX() evts_irqevt(EVTS_IRQ_UART0TX,EVTOFS_SERPKT+1)




// -- evts --
#define postevt evts_post


#define EVTS_QSIZE  16
#define EVTS_EVTSHIFT 3
#define EVTS_ADD (1<<EVTS_EVTSHIFT)

#define EVTOFS_MAIN   0x00
#define EVTOFS_LBDPKT (EVTOFS_MAIN   + EVTS_ADD)
#define EVTOFS_SERPKT (EVTOFS_LBDPKT + EVTS_ADD)
#define EVTOFS_PKT    (EVTOFS_SERPKT + EVTS_ADD)
#define EVTOFS_SCK    (EVTOFS_PKT + EVTS_ADD)

#define EVTS_TABLELIST   \
{                        \
   &main_evts          \
  ,&lbdpkt_evts        \
  ,&serpkt_evts        \
  ,&pkt_evts           \
  ,&sck_evts           \
}


#define EVTS_IRQ_UART0RX  0
#define EVTS_IRQ_UART0TX  1
#define EVTS_IRQSIZE      2


// -- ifs --
#define NUM_IFS 2



// -- pkt --
#define PKT_FIFO_NUM  10


//pq packetqueues
#define PQ_NUM 10

//buf
#define BUF_SIZE 20
#define BUF_NUM 5



#endif

