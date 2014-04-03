
#include "uart.h"
#include "circularBuffer.h"
#include <avr/io.h>
#include "mudem.h"
#include "main.h"





uint8_t uart0_in[M_CircularBufferCalcRequiredMemorySizeForBuffer(5)];
uint8_t uart0_out[M_CircularBufferCalcRequiredMemorySizeForBuffer(5)];










unsigned char s_ocr2 = 16000/125;


void updateOCR2(void)
{
  OCR2A = s_ocr2-1;  // compare A
  OCR2B = (s_ocr2/2)-1;  // compare B
}


void setupOCR2(void)
{
  DDRB |= (1<<PB3);    // set PB3 to output  (high)
  DDRD |= (1<<PD3);    // set PD3 to output  (high)
//  DDRD |= (1<<PD5);    // set PD5 to output  (high)
//  DDRD |= (1<<PD6);    // set PD6 to output  (high)


  TCCR2B=0;
  TCCR2A=0;
  //Set Initial Timer value
  TCNT2=0;
  //clear interrupt flags to avoid any pending interrupts
  TIFR2=(1<<OCF2A)|(1<<OCF2B)|(1<<TOV2);


  TIMSK2= 0;//(1<<ICIE1)|(1<<TOIE1); // Capture  OV  IRQ
  
  // Set edge capture direction and clock divider:
  TCCR2A = 0x40|   // toggle OC0A on compare A match
           0x20|   //  compare B mode
           0x03;   // mode
  TCCR2B = 0x01|   // clock devide by 1
           0x08;   // mode
           
  updateOCR2();
}


void setupADC(void)
{
  ADCSRA= 0x80|  // Enable
          0x20|  // Auto Trigger
          0x08|  // Enable interrupts
//          0x01;  // ClkDiv 2
          0x02;  // ClkDiv 4
//          0x03;  // ClkDiv 8
//          0x04;  // ClkDiv 16
//          0x05;  // ClkDiv 32
//          0x06;  // ClkDiv 64
//          0x07;  // ClkDiv 128
          
  DDRD &= ~(1<<PD2);    // set PD2 to input

//  ADCSRB= 0x00;  //freerunning
//  ADCSRB= 0x01;  //trigger from acomp
  ADCSRB= 0x02;  //trigger from extint0
//  ADCSRB= 0x03;  //trigger from TC0_CMA
//  ADCSRB= 0x04;  //trigger from TC0_OV

  ADMUX = 0x40|  //VREF
          0x20|  //LeftAdjust
          0x00;  //Input=AD0
          
//  ADCSRA|=0x40;  


// Configure Analog comparator. Used for ADC trigger.
//  ACSR  = 0x04|  // 
//          0x00;  // Rising edge

   ADCSRA |= 0x40;
}

static long samples=0;
static char pathcheck=1;

ISR(INT0_vect, ISR_NAKED)
{
  reti();
}


ISR(ADC_vect)///, ISR_NAKED)
{
  //UDR0 = (ADCH>>2)|0x40;
//  reti();
}



int main(int argc, char * argv[])
{
  char count1;
  
  CircularBufInit(uart0_in,sizeof(uart0_in));
  CircularBufInit(uart0_out,sizeof(uart0_out));
  
  Uart0_Init();
//  Uart0_SetBaudrate(115200);
  Uart0_SetBaudrate(19200);
  Uart0_SetFormat(8,1,2);
  
  sei();
  
#if 0
  DIDR0=0x01; //Disable digital input buffers of ADC analog input pins.
  DIDR1=0x03; //Disable digital input buffers of ACOMP analog input pins.
 
  setupOCR2();
  
  EICRA = 0x03; //external interrupt
  EIMSK = 1;
  EIFR  = 1; // Remove possible interrupt from INT0
  setupADC();
#endif
  
  count1 = 0;

#if 1
  CircularBufWrite(uart0_out,'2');
  CircularBufWrite(uart0_out,'3');
  CircularBufWrite(uart0_out,'4');
  Uart0_StartTx();
#endif  

  
  while(1)
  {
    uint16_t w;
    
    if (CircularBufNotEmpty_INLINE(uart0_in))
    {
      uint8_t c;

      c = CircularBufRead(uart0_in);
      mudem_ByteReceived(c);
    }
    
    while ( CircularBufNotFull_INLINE(uart0_out) && (w = mudem_GetOut()))
    {
      CircularBufWrite(uart0_out,w);
      Uart0_StartTx();
    }
    
  //  dht11_start();

    
#if 0
    if(ev == serial_line_event_message)
    {
       char * pc=(char*)data;
       char c;
       
//       printf("received line: %s\n", pc);
       while (c=*pc++)
       {
         if (c=='-')
         {
           s_ocr2--;
           updateOCR2();
         }
         if (c=='+')
         {
           s_ocr2++;
           updateOCR2();
         }
         
         
//         printf("%dkHz",16000/s_ocr2);
       }
       
    }
#endif    
    
  }
  
}





/*
0x01,  //dst address
0x01,  //msglen
0x01,  //src address
0x01,  //dst port
0x01,  //src port
  0x01,  //[ resp | ack | err | 0 | act | 3*adrlen ]
  0x01,  //[  4 * getlen  | 4 * setlen ]
  0x0n,  //adr
  0x0m,  //setdat
0x01,  //checksum
*/



static uint8_t devdat[2];


static uint8_t recvPos=0;
static uint8_t msgLen=0;
static uint8_t srcAdr=0x41;
static uint8_t srcPort=0;
static uint8_t len1=0;
static uint8_t len2=0;
static uint8_t adr=0;



static uint8_t outPos=0;

uint16_t main_Txd(void)
{
  uint8_t c;
  
  if (outPos<=9)
  {
    switch (outPos)
    {
      case 0:
        c=srcAdr;
        break;
      case 1:
        c=8;
        break;
      case 2:
        c=0x40;
        break;
      case 3:
        c=srcPort;
        break;
      case 4:
        c=0x40;
        break;
      case 5:
        c=len1|0x80;
        break;
      case 6:
        c=len2>>4;
        break;
      case 7:
        c=adr;
        break;
      case 8:
        if (len2&0x0f)
        {
          c=devdat[adr];
          break;
        }
        outPos++;
      case 9:
        c='!';
        break;
      default:
        c='?';
        break;
    }
  
  
    outPos++;
    return c | MUDEM_DATA;
  }
  else
    return 0;
}


void main_Recv(uint16_t w)
{
  if ( w & MUDEM_DATA )
  {
    uint8_t c = w;
    switch (recvPos)
    {
      case 0: //dst
        if (c!=0x40)
          recvPos=250;
        break;
      case 1://msglen
        msgLen = c;
        break;
      case 2://src
        srcAdr = c;
        break;
      case 3://dstport
        if (c!=0x40)
          recvPos=250;
        break;
      case 4://srcprt
        srcPort = c;
        break;
      case 5://len1
        if ((c&0x07)!=0x01)  //1 byte address
          recvPos=250;
        else
          len1=c;  
        break;
      case 6://len2
        if (c&0xee)  //1 byte get;1 byte set
          recvPos=250;
        else
          len2=c;
        break;
      case 7://adr
        adr=c&1;
        break;
      case 8://dat
        if (len2&1)
        { // setdat
          devdat[adr]=c;
          break;
        }
        recvPos++;
      case 9:// chksum
        break;
                
      default:
        recvPos=250;  
        break;
    }
    
      CircularBufWrite(uart0_out,recvPos+'0');
      Uart0_StartTx();
    
    recvPos++;
  }
  else if (w & MUDEM_CTRL)
  {
    uint8_t c = w;
    if (c==MUDEM_CTRL_START)
    {
      recvPos=0;
    }
  }
}









uint16_t dbg_Txd(void)
{
  return MUDEM_NODATA;
}


void dbg_Recv(uint16_t w)
{

}

/*---------------------------------------------------------------------------*/

