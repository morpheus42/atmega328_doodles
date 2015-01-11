/* Code licensed under GPL3. See license.txt  */


#include "config.h"
#include "serpkt.h"
#include "stdint.h"

#include "evts.h"
#include "uart.h"
#include "circularBuffer.h"




#define STATE_INFRAME  0x01
#define STATE_INESC    0x02
#define STATE_GOTLEN   0x04
#define STATE_GOTADR   0x08


#define CHARCODE_FRAME    0x01
#define CHARCODE_ESC      0x1B
#define CHARCODE_ESCAPED  0x40
#define CHARCODE_NOTNULL  0xDB



uint8_t uart0_in[M_CircularBufferCalcRequiredMemorySizeForBuffer(5)];
uint8_t uart0_out[M_CircularBufferCalcRequiredMemorySizeForBuffer(5)];


static pkt_xferadm_t * rxadm;
static pkt_xferadm_t * txadm;

static uint8_t * rxbuf;
static uint8_t rxlen;
static uint8_t rxstate;
static uint8_t * txbuf;
static uint8_t txlen;
static uint8_t txstate;
static uint8_t myaddr='?';//'~';


static uint8_t oc=1;

static void rxchar(void)
{
  uint8_t c;

  while ((c=CircularBufRead(uart0_in)))
  { // 0 not used in uartdata.
    // 0 happens to be also the queue empty value ;-)

//printf("%x:%d.\n",c,rxlen);

    
    if (c==CHARCODE_FRAME) // frame key
    {
      if (rxstate & STATE_INFRAME)
      { // we were in a frame!
        rxstate=0;
        rxadm->len=rxbuf-rxadm->buf;
        rxadm->cb(rxadm);
      }
      //start of frame
      rxlen = rxadm->len;
      if (rxlen)
      {
        rxstate=STATE_INFRAME;
        rxbuf = rxadm->buf;
      }
    }
    else
    {
        if (c==CHARCODE_ESC) // escape char !!
        {
          rxstate |= STATE_INESC;
        }
        else
        {
          if (rxstate & STATE_INESC) // previous char was escape?
          {
            rxstate ^= STATE_INESC;
            c^=CHARCODE_ESCAPED;
          }
        
          if (c==CHARCODE_NOTNULL)
          {
            c=0;
          }
          
          if (rxstate & STATE_GOTADR)
          {
            if (rxlen>0)
            {
              *rxbuf=c;
              rxbuf++;
              rxlen--;
              if (rxlen==0)
              {
                rxstate=0;
                rxadm->len=rxbuf-rxadm->buf;
                rxadm->cb(rxadm);
              }
            }        
          }
          else
          {
            if (rxstate & STATE_GOTLEN)
            {
              if ((myaddr==0)||(c==myaddr)||(c==0x00)||(c==0xff))
              {
                *rxbuf=c;
                rxbuf++;
                rxlen--;
                rxstate |= STATE_GOTADR;
              }
              else
                rxstate=0;
            }
            else
            {
              if (c<rxlen)
              {
                rxlen=c;
                rxstate |= STATE_GOTLEN;
              }
            }
          }
        }          
      
    }    
  }

}



static void txchar(void)
{
  uint8_t newstate=txstate;
  
  if (txbuf)
  {
    while (txlen)
    {
      uint8_t c=*txbuf;
             
      if ((newstate & STATE_GOTADR)==0)
      {
        if ((newstate & STATE_INFRAME)==0)
        {
          c=CHARCODE_FRAME;
          newstate=STATE_INFRAME;
        }
        else if ((newstate & STATE_GOTLEN)==0)
        {
          c=txlen;
          newstate|=STATE_GOTLEN;
        }
        else if ((newstate & STATE_GOTADR)==0)
        {
          if (c==0xff)
            c=myaddr;
          newstate|=STATE_GOTADR;
        }
      }


      if (newstate & STATE_GOTLEN)
      {
        if ((c==CHARCODE_FRAME)||(c==CHARCODE_ESC)||(c==CHARCODE_NOTNULL))
        {
          if (newstate & STATE_INESC)
            c ^= CHARCODE_ESCAPED;
          else
            c = CHARCODE_ESC;  
          newstate ^= STATE_INESC;
        }
      }
      
      
      if (c==0)
        c=CHARCODE_NOTNULL;
      

  //printf("<[%x,%x,%x].\n",c,*txbuf,txbuf);
      if (!CircularBufWrite(uart0_out,c))
      {
        break;
      }

      if ((newstate & (STATE_GOTADR|STATE_INESC))==STATE_GOTADR)
      {
        txbuf++;
        txlen--;
        if (!txlen)
        {
          txadm->cb(txadm);
          txbuf = txadm->buf;
          txlen = txadm->len;
          break;
        }
      }

      txstate=newstate;
    }
    Uart0_StartTx();
  }
  
}


pkt_SetReceiveBuf_ft serpkt_SetReceiveBuf;
int8_t serpkt_SetReceiveBuf(pkt_xferadm_t * adm)
{
  if (rxadm)
    return -1;
  rxadm = adm;
  
  return 1;
}


pkt_SetTransmitBuf_ft serpkt_SetTransmitBuf;
int8_t serpkt_SetTransmitBuf(pkt_xferadm_t * adm)
{
  if (txbuf)
    return -1;
  txadm = adm;
  txbuf = adm->buf;
  txlen = adm->len;
  Uart0_StartTx();
  
  return 1;
}


void serpkt_Init(void)
{
  CircularBufInit(uart0_in,sizeof(uart0_in));
  CircularBufInit(uart0_out,sizeof(uart0_out));
    
  Uart0_Init();
//  Uart0_SetBaudrate(115200);
  Uart0_SetBaudrate(19200);
  Uart0_SetFormat(8, 1, 2);  
}




const evtsfun_t * serpkt_evts[SERPKT_EVT_NUM] =
{
  rxchar,
  txchar,
};


const pkt_if_t serpkt_if=
{
  serpkt_evts,
  serpkt_Init,
  serpkt_SetReceiveBuf,
  serpkt_SetTransmitBuf,
  &myaddr
};



