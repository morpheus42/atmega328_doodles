


#ifdef __cplusplus
extern "C" {
#endif
  
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
void CircularBufPush(void * vp, unsigned char v);




/*
  Get oldest item from buffer:
    vp: pointer to buffer
   return:  >=0 : oldest item
            <0  : error (not a buffer item)
*/
int CircularBufPop(void * vp);

#ifdef __cplusplus
} //extern C
#endif

