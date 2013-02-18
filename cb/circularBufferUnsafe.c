


#include "circularBuffer.h"


typedef struct
{
  unsigned char  head;
  unsigned char  tail;
  unsigned char  len;
} CircularBufHdrType;


/*
  vp: pointer to memory for new circular buffer
  len: size of memory
*/
void CircularBufInit(void * vp, unsigned char len)
{
  CircularBufHdrType * p = (CircularBufHdrType *)vp;
  p->head=sizeof(CircularBufHdrType);
  p->tail=sizeof(CircularBufHdrType);
  p->len=len;
}


/*
  Add new item to buffer:
    vp: pointer to buffer
    v:  value to put in buffer
*/
void CircularBufPush(void * vp, unsigned char v)
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


/*
  Get oldest item from buffer:
    vp: pointer to buffer
   return:  >=0 : oldest item
            <0  : error (not a buffer item)
*/
int CircularBufPop(void * vp)
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

