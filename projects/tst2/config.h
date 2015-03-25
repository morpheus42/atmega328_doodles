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
#define UART0_RX(p)   {CircularBufWrite_INLINE(uart0_in,p);evts_irqevt(EVTS_IRQ_UART0RX,serpkt_evt_rx);}
#define UART0_TX()    CircularBufRead_INLINE(uart0_out)
#define UART0_TXQNE() CircularBufNotEmpty_INLINE(uart0_out)
#define UART0_EOFTX() evts_irqevt(EVTS_IRQ_UART0TX,serpkt_evt_eoftx)
extern uint8_t serpkt_evt_rx;
extern uint8_t serpkt_evt_eoftx;



// -- evts --
#define postevt evts_post

#define EVTS_QSIZE  16

#define EVTS_IRQ_UART0RX  0
#define EVTS_IRQ_TICP     1
#define EVTS_IRQ_UART0TX  2
#define EVTS_IRQSIZE      3


// -- tmr --
#define NUM_TMR 6
#define TMR_TOKEN_EXPIRED(p) evts_post(p)



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

