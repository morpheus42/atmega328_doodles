/* Code licensed under GPL3. See license.txt  */


#ifndef __BUFFL_H__
#define __BUFFL_H__

#include "config.h"
#include "stdint.h"

typedef int8_t buf_handle_t;

#define BUF_HANDLE_IS_VALID(h) (h>=0)


#define Buf_Init() {}


#define buf_Getv(size) buf_Get()

buf_handle_t buf_Getv(void);

void buf_Free( buf_handle_t h);

uint8_t * buf_Ptr(buf_handle_t h);

#define buf_Size(id) (BUF_SIZE)


#endif


