

#ifndef _CIRCULARBUFFER_H_
#define _CIRCULARBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef struct
{
  unsigned char  head;
  unsigned char  tail;
  unsigned char  len;
} CircularBufHdrType;


#define M_CircularBufferCalcRequiredMemorySizeForBuffer(size) (size+3)


/*
  vp: pointer to memory for new circular buffer
  len: size of memory
*/
void CircularBufInit(void * vp, unsigned char len);




/*
  Add new item to buffer:
    vp: pointer to buffer
    v:  value to put in buffer
*/
uint8_t CircularBufWrite(void * vp, unsigned char v);

extern inline uint8_t CircularBufWrite_INLINE(void * vp, unsigned char v)
{
  CircularBufHdrType * p = (CircularBufHdrType *)vp;
  uint8_t x=p->head;
  uint8_t y=x+1;

  if (y>=p->len)
  {
    y=sizeof(CircularBufHdrType);
  }
  if (p->tail!=y)
  {
    ((uint8_t*)p)[x]=v;
    p->head=y;
    return 1;
  }
  return 0;
}




char CircularBufNotEmpty(void * vp);


extern inline char CircularBufNotEmpty_INLINE(void * vp)
{
  CircularBufHdrType * p = (CircularBufHdrType *)vp;

  return ((p->head)-(p->tail));
}



/*
  Get oldest item from buffer:
    vp: pointer to buffer
   return:  >=0 : oldest item
            <0  : error (not a buffer item)
*/

uint8_t CircularBufRead(void * vp);

extern inline uint8_t CircularBufRead_INLINE(void * vp)
{
  CircularBufHdrType * p = (CircularBufHdrType *)vp;

  if (p->head!=p->tail)
  {
    unsigned char x = p->tail;
    unsigned char dat = ((unsigned char*)p)[x];

    x++;
    if (x>=p->len)
    {
      x=sizeof(CircularBufHdrType);
    }
    p->tail=x;
    return dat;
  }
  return '?';
}




#ifdef __cplusplus
} //extern C
#endif


#endif //_CIRCULARBUFFER_H_
