/* Code licensed under GPL3. See license.txt  */

#ifndef _DE_AVR_MSECTIONS_H_
#define _DE_AVR_MSECTIONS_H_

#include <avr/pgmspace.h>

#define INTEXT PROGMEM
#define ReadU8FromText(adr)  pgm_read_byte_near(adr)
#define ReadU16FromText(adr) pgm_read_word_near(adr)
#define ReadU32FromText(adr) pgm_read_dword_near(adr)
#define ReadVptrFromText(adr) pgm_read_word_near(adr)

#endif

