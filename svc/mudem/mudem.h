

#ifndef _MUDEM_H_
#define _MUDEM_H_

#include "stdint.h"

typedef void ByteReceived_t(uint8_t b);

typedef struct
{
  ByteReceived_t * pfnByteReceived;
  ByteReceived_t * pfnControl;
}mudem_channel_t;

extern mudem_channel_t mudem_channels[8];

void mudem_ByteReceived(uint8_t b);



#endif
