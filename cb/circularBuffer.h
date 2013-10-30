

#ifndef _CIRCULARBUFFER_H_
#define _CIRCULARBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  unsigned char  head;
  unsigned char  tail;
  unsigned char  len;
} CircularBufHdrType;


#define M_CircularBufferCalcRequiredMemorySizeForBuffer(size)  (size+3)


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
void CircularBufWrite(void * vp, unsigned char v);

#define CircularBufWrite_INLINE( vp, v ) { \
  CircularBufHdrType * p = (CircularBufHdrType *)vp;     \
  unsigned char x=p->head+1;                             \
                                                         \
  if (x>=p->len)                                         \
  {                                                      \
    x=sizeof(CircularBufHdrType);                        \
  }                                                      \
  if (p->tail!=x)                                        \
  {                                                      \
    p->head=x;                                           \
    ((unsigned char*)p)[x]=v;                            \
  }                                                      \
}


/*
  Get oldest item from buffer:
    vp: pointer to buffer
   return:  >=0 : oldest item
            <0  : error (not a buffer item)
*/
int CircularBufRead(void * vp);




#ifdef __cplusplus
} //extern C
#endif


#endif //_CIRCULARBUFFER_H_
