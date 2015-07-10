/* Code licensed under GPL3. See license.txt  */

#ifndef _DE_LNX_MSECTIONS_H_
#define _DE_LNX_MSECTIONS_H_

typedef void __MSECTIONSVFUNCV__( void );
//#include "pgmspace.h"
#define INTEXT
#define ReadU8FromText(adr)  (*((uint8_t*)(adr)))
#define ReadU16FromText(adr) (*((uint16_t*)(adr)))
#define ReadU32FromText(adr) (*((uint32_t*)(adr)))
#define ReadVptrFromText(adr) (*((__MSECTIONSVFUNCV__**)(adr)))

#endif

