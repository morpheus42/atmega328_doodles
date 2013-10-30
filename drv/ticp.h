


#ifndef __TICP_H__
#define __TICP_H__

#include "circularBuffer.h"


#define TICP_EDGE_FALL_START 0x00
#define TICP_EDGE_RISE_START 0x40
#define TICP_EDGE_BOTH       0x20


#define TICP_CLK_NONE     0
#define TICP_CLK_DIV_1    1
#define TICP_CLK_DIV_8    2
#define TICP_CLK_DIV_64   3
#define TICP_CLK_DIV_256  4
#define TICP_CLK_DIV_1024 5
#define TICP_CLK_T1_FALL  6
#define TICP_CLK_T1_RISE  7


#define ticp_init()

void ticp_config(void * buf, char cfg);



#endif

