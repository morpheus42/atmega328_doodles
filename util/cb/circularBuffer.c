


#include "circularBuffer.h"


/*
  vp: pointer to memory for new circular buffer
  len: size of memory
*/
void CircularBufInit(void * vp, unsigned char len)
{
  CircularBufHdrType * p = (CircularBufHdrType *)vp;
  p->head=sizeof(CircularBufHdrType);
  p->tail=sizeof(CircularBufHdrType);
  p->len=len-sizeof(CircularBufHdrType);
}


/*
  Add new item to buffer:
    vp: pointer to buffer
    v:  value to put in buffer
*/
void CircularBufWrite(void * vp, unsigned char v)
{
  CircularBufWrite_INLINE(vp,v);
}

/*
  Get oldest item from buffer:
    vp: pointer to buffer
   return:  >=0 : oldest item
            <0  : error (not a buffer item)
*/
uint8_t CircularBufRead(void * vp)
{
  return CircularBufRead_INLINE(vp);
}





