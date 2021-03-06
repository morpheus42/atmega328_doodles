/* Code licensed under GPL3. See license.txt  */

#ifndef _LTENUM_H_
#define _LTENUM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "compdefs.h"
#include "msections.h"


#ifndef QUOTE
#define QUOTE(n) #n
#endif


#define LTE_REFGROUP(group) extern void * __start___lte_##group 
#define LTE_GROUPOFS(group) &__start___lte_##group

#define LTE_ADDENTRY(typ, group, name) typ __attribute((used,__section__(QUOTE(__lte_##group)))) __lte_##group##_##name

//#define LTE_ENTRYSET(group, name, val) __lte_##group##_##name = val
#define LTE_ENTRYADR(group, name) __lte_##group##_##name

#define LTE_ENTRYREAD8(group, name) ReadU8FromText(__lte_##group##_##name)
#define LTE_ENTRYREAD16(group, name) ReadU16FromText(__lte_##group##_##name)

#define LTE_ReadU8FromGroup(group, idx) ReadU8FromText(((uint8_t*)&__start___lte_##group)+idx)
#define LTE_ReadU16FromGroup(group, idx) ReadU16FromText(((uint8_t*)&__start___lte_##group)+idx)
#define LTE_ReadVptrFromGroup(group, idx) ReadVptrFromText(((uint8_t*)&__start___lte_##group)+idx)

#define LTE_ENTRYIDX(group, name) OFS_LOW8((void *)&__lte_##group##_##name)



#ifdef __cplusplus
} //extern C
#endif


//#define LTE_DEFGROUP(group) char * __start_##group __attribute__((aligned(0x100)))
//__attribute__((at(0)))

//__attribute((__section__(QUOTE(__lte_##group))))

//#define LTE_ENTRYVALUE(group, name) (&__lte_##name)
// __attribute__((always_inline));

//#define LTE_ENTRYVALUE3(dst, group, name) asm("ldi %0,lo8(%1);":"=r"(dst):"i"(&__lte_##name):)

#endif //_LTENUM_H_


