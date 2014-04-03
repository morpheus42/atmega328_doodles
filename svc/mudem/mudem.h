

#ifndef _MUDEM_H_
#define _MUDEM_H_

#include "stdint.h"


#define MUDEM_NODATA     0x000
#define MUDEM_DATA       0x100
#define MUDEM_CTRL       0x200

#define MUDEM_CTRL_START    0x01
#define MUDEM_CTRL_CONTINUE 0x02



typedef void ByteReceived_t(uint16_t b);
typedef uint16_t ByteTransmit_t(void);

typedef struct
{
  ByteReceived_t * pfnByteReceived;
  ByteTransmit_t * pfnByteTransmit;
}mudem_channel_t;

extern mudem_channel_t mudem_channels[8];

void mudem_ByteReceived(uint8_t b);

uint16_t mudem_GetOut(void);



#endif
