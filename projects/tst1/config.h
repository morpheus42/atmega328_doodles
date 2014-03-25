#ifndef _CONFIG_C_
#define _CONFIG_C_

#include "stdint.h"
#include "circularBuffer.h"
extern uint8_t uart0_out[];
extern uint8_t uart0_in[];
#define UART0_RX(p)   CircularBufWrite_INLINE(uart0_in,p)
#define UART0_TX()    CircularBufRead_INLINE(uart0_out)
#define UART0_TXQNE() CircularBufNotEmpty_INLINE(uart0_out)



#endif

