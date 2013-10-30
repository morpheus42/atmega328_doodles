


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
CircularBufWrite_INLINE(vp,v)
#if 0
{
  CircularBufHdrType * p = (CircularBufHdrType *)vp;
  unsigned char x=p->head+1;

  if (x>=p->len)
  {
    x=sizeof(CircularBufHdrType);
  }
  if (p->tail!=x)
  {
    p->head=x;
    ((unsigned char*)p)[x]=v;
  }    
}
#endif

/*
  Get oldest item from buffer:
    vp: pointer to buffer
   return:  >=0 : oldest item
            <0  : error (not a buffer item)
*/
int CircularBufRead(void * vp)
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
  return -1;
}






