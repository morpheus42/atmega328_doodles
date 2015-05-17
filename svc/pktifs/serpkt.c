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


static pkt_xferadm_t rxadm;
static pkt_xferadm_t txadm;

static uint8_t myIfId;

//{
static uint8_t * rxbuf;
static uint8_t rxlen;
static uint8_t rxstate;
static uint8_t txstate;
static uint8_t myaddr=0;
//}

uint8_t serpkt_evt_rx=0;
uint8_t serpkt_evt_eoftx=0;


static void rxchar(void)
{
  uint8_t c;

  while ((c=CircularBufRead(uart0_in)))
  { // 0 not used in uartdata.
    // 0 happens to be also the queue empty value ;-)

    if (c==CHARCODE_FRAME) // frame key
    {
      if (rxstate & STATE_INFRAME)
      { // we were in a frame!
        rxstate=0;
        rxadm.len=rxbuf-rxadm.buf;
        pktifs_PktReceived(&rxadm);
        
      }
      //start of frame
      pktifs_GetRxPkt(myIfId, &rxadm);
      rxlen = rxadm.len;
      if (rxlen)
      {
        rxstate=STATE_INFRAME;
        rxbuf = rxadm.buf;
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
          else if (c==CHARCODE_NOTNULL)
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
                rxadm.len=rxbuf-rxadm.buf;
                pktifs_PktReceived(&rxadm);
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
    
  if (!txadm.buf)
  {
    // get new from queue
    pktifs_GetTxPkt(myIfId,&txadm);
  }
  
  if (txadm.buf)
  {
    while (txadm.len)
    {
      uint8_t c=*(txadm.buf);
             
      if ((newstate & STATE_GOTADR)==0)
      {
        if ((newstate & STATE_INFRAME)==0)
        {
          c=CHARCODE_FRAME;
          newstate=STATE_INFRAME;
        }
        else if ((newstate & STATE_GOTLEN)==0)
        {
          c=txadm.len;
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
      

      if (!CircularBufWrite(uart0_out,c))
      {
        break;
      }

      if ((newstate & (STATE_GOTADR|STATE_INESC))==STATE_GOTADR)
      {
        txadm.buf++;
        txadm.len--;        
        if (!txadm.len)
        {
//          txadm->cb(txadm);
          txstate=0;
//          txbuf = txadm->buf;
//          txlen = txadm->len;
          pktifs_PktTransmitted(&txadm);
          break;
        }
      }

      txstate=newstate;
    }
    Uart0_StartTx();
  }
  
}




static pkt_TxPktInQ_ft TxPktInQ;
static void TxPktInQ(void)
{
  Uart0_StartTx();
}



EVTS_DEF_FUNC(serpkt_rxchar,rxchar);
EVTS_DEF_FUNC(serpkt_txchar,txchar);


void serpkt_Init(uint8_t id)
{
  myIfId = id;

  CircularBufInit(uart0_in,sizeof(uart0_in));
  CircularBufInit(uart0_out,sizeof(uart0_out));
  
  serpkt_evt_rx = evts_DefToNr(serpkt_rxchar);
  serpkt_evt_eoftx = evts_DefToNr(serpkt_txchar);
  
    
  Uart0_Init();
//  Uart0_SetBaudrate(115200);
  Uart0_SetBaudrate(19200);
  Uart0_SetFormat(8, 1, 2);  
}


static void SetAdr(uint8_t newAdr)
{
  myaddr = newAdr;
}



const pkt_if_t serpkt_if=
{
  serpkt_Init,
  TxPktInQ,
  SetAdr,
  &myaddr
};



