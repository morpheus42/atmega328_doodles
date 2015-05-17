/* Code licensed under GPL3. See license.txt  */

#ifndef _DE_AVR_COMPDEFS_H_
#define _DE_AVR_COMPDEFS_H_

#define ALWAYS_INLINE __attribute__ ((always_inline)) extern inline




ALWAYS_INLINE uint8_t OFSLOW8(void * p)
{
  uint8_t r;
  asm ("ldi %0,lo8(%1);":"=r"(r):"i"(p):);
  
  return r;
}

#define OFS_LOW8(x) OFSLOW8(&x)



#endif

