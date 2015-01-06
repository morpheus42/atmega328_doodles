/* Code licensed under GPL3. See license.txt  */

// fixed length version

#include "config.h"
#include "buffl.h"
#include "string.h"


typedef uint8_t buf_t[BUF_SIZE];

buf_t __buffl_buf[BUF_NUM];
uint8_t __buffl_used[(BUF_NUM/8)+1];


uint8_t * buf_Ptr(buf_handle_t h)
{
  if ((unsigned)h>=BUF_NUM)
    return 0;
  return __buffl_buf[h];
}


buf_handle_t buf_Getv(void)
{
  uint8_t * pf = __buffl_used;
  uint8_t   m=1;
  uint8_t   ofs=0;
  
  while (ofs<BUF_NUM)
  {
    if (!((*pf)&m))
    {
      *pf^=m;
      memset(buf_Ptr(ofs), 'n', BUF_SIZE);      
      return ofs;
    }
    m=m<<1;
    if (!m)
    {
      m=1;
      pf++;
    }
    ofs++;
  }
  return -1;
}

void buf_Free( buf_handle_t h)
{
  if (BUF_NUM>7)
  {
    __buffl_used[h>>3] &= ~(1<<(h&7));
  }
  else
  {
    __buffl_used[0] &= ~(1<<h);
  }
  memset(buf_Ptr(h),BUF_SIZE,'f');
}





