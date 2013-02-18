/*
 */

#include "circularBuffer.h"

//char inpin=2;
#define inpin 8

//char start0pin=A4;
#define start0pin A4

#define dbg0pin 7

char ledpin = 13;

char state = 0;
char lwc=0;

/*
typedef struct
{
  unsigned char  head;
  unsigned char  tail;
  unsigned char  len;
} CircularBufType;
*/

unsigned char in0times[M_CircularBufferCalcRequiredMemorySizeForBuffer(80)];
unsigned char serout[M_CircularBufferCalcRequiredMemorySizeForBuffer(240)];




void QueueChar2Uart(unsigned char c)
{
  CircularBufPush(serout,c);
  UCSR0B |= (1 << UDRIE0);
//  while ((UCSR0A & (1<<UDRE0))==0);
//  UDR0 = c;
}

const unsigned char dgt[]="0123456789ABCDEF";
void QueueCharAsHex2Uart(unsigned char c)
{
  CircularBufPush(serout,dgt[c>>4]);
  CircularBufPush(serout,dgt[c&15]);
  UCSR0B |= (1 << UDRIE0);
//  while ((UCSR0A & (1<<UDRE0))==0);
//  UDR0 = c;
}


/*
  4800  2,1,160,184,6   //416
  9600  2,0,207,184,6
 19200  2,0,103,184,6
 38400  2,0,51,184,6 
 57600  0,0,16,184,6
115200  2,0,16,184,6
*/


void InitUart(long br)
{
//   int d = ((1843200/8)/br);
//   int d = ( (F_CPU/8) / br);
   int d = ( (F_CPU/8) / br)-1;

#if 0
   Serial.print("\nU");Serial.print(UCSR0A);
   Serial.print(",");Serial.print(UBRR0H);
   Serial.print(",");Serial.print(UBRR0L);
   Serial.print(",");Serial.print(UCSR0B);
   Serial.print(",");Serial.print(UCSR0C);
   
   Serial.print("\nN");Serial.print((1<<U2X0));
   Serial.print(",");Serial.print(d>>8);
   Serial.print(",");Serial.print(d&255);
   Serial.print(",");Serial.print((1<<RXEN0)|(1<<TXEN0));
   Serial.print(",");Serial.print((3<<UCSZ00));
   Serial.print(",");Serial.print(d);
#else    

   UCSR0A = (1<<U2X0);
   UBRR0H = d>>8;
   UBRR0L = d;
   UCSR0B = (1<<RXEN0)|(1<<TXEN0);
   UCSR0C = (3<<UCSZ00);
#endif 
}




void T1Init(void)
{
  TCCR1B=0;
  TCCR1A=0;
  //Set Initial Timer value
  TCNT1=0;
  //clear interrupt flags to avoid any pending interrupts
  TIFR1=(1<<ICF1)|(1<<TOV1);

  //First capture on rising edge
  //TCCR1B|=(1<<ICES1);
  //First capture on falling edge
  TCCR1B&=~(1<<ICES1);

  //Enable input capture and overflow interrupts
  TIMSK1|=(1<<ICIE1)|(1<<TOIE1);  
}

void T1SetDiv(short dv)
{
  char cs=0;
  
  switch (dv)
  {
    case 1:
      cs=1;
      break;
    case 8:
      cs=2;
      break;
    case 64:
      cs=3;
      break;
    case 256:
      cs=4;
      break;
    case 1024:
      cs=5;
      break;
    case -1:
      cs=6;
      break;
    case -2:
      cs=7;
      break;
  }
  TCCR1B = (TCCR1B & 0xf8) | cs;
  sei();
}

//void T1SetCnt(unsigned short cnt)
//{
//  TCNT1 = cnt;
//}
#define T1SetCnt(p) TCNT1=p

//unsigned short T1GetCnt(void)
//{
//  return TCNT1;
//}
#define T1GetCnt() TCNT1







void setup()
{                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
//  Serial.begin(9600);
//  Serial.print("\nSetup{");
  CircularBufInit(serout,sizeof(in0times));
//  InitUart(38400);
  InitUart(115200);
  CircularBufInit(in0times,sizeof(in0times));
  pinMode(ledpin, OUTPUT);
  pinMode(start0pin, OUTPUT);
  digitalWrite(start0pin, HIGH);
  pinMode(inpin, INPUT);
  digitalWrite(inpin, HIGH);
  pinMode(dbg0pin, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
//  attachInterrupt(0, irqP2, CHANGE);
  T1Init();
  T1SetDiv(8);
//  Serial.print("}");

}



void startReq(char i)
{
  switch (i)
  {
    case 0:
      digitalWrite(inpin, LOW);
      pinMode(inpin, OUTPUT);
      digitalWrite(inpin, LOW);
      delay(18);
      digitalWrite(inpin, HIGH);
      pinMode(inpin, INPUT);
      digitalWrite(inpin, HIGH);
      break;
    case 1:
      pinMode(start0pin, OUTPUT);
      digitalWrite(start0pin, LOW);
      delay(18);
      digitalWrite(start0pin, HIGH);
      pinMode(start0pin, INPUT);
      break;
  }
  
}



//#define PrintChar(c) (void)c
//#define PrintChar(c) Serial.print(c)
#define PrintChar(c) QueueChar2Uart(c)
#define PrintCharAsHex(c) QueueCharAsHex2Uart(c)

void loop()
{
  unsigned char c0,c1;
  unsigned char w;
  unsigned char cnt;
  
  PrintChar('\n');
  PrintChar(':');
  
  startReq(1);
//  delay(2);
//  TIMSK0 &= 0xfe;
  
  c1='?';
  c0=200;
  cnt=0;
  while (c0>0)
  {
    short x=CircularBufPop(in0times);
    c0--;

    if (x>=0)
    {
      c0=127;
#if 0
      if (x&0x80)
        PrintChar('H');
      else
        PrintChar('L');
      x&=127;
      if (x!=127)
      {        
        PrintCharAsHex(x);
      }
      else
        PrintChar('\n');
#else
      if (x&0x80)
      {
        x&=0x7f;
        if (x<20)
          c1='*';
        else if (x<33)
          c1='0';
        else if (x<65)
          c1='m';
        else if (x<75)
          c1='1';
        else if (x<85)
        {
          c1='s';
        }
        else
          c1='H';
      }
      else
      {
        if (x<45)
          c1='<';
        else if (x<60)
          c1=' ';
        else if (x<75)
          c1='>';
        else if (x<85)
        {
          c1='S';
        }
        else
        {
          c1='L';
        }

      }
      switch (c1)
      {
        case '0':
          w=(w*2);
          cnt++;
          break;
        case '1':
          w=(w*2)+1;
          cnt++;
          break;
        case 'S':
          cnt=0;
          w=0;
          break;        
      }
//      PrintChar(c1);
      
      switch (cnt)
      {
         case 8:
         case 16:
         case 24:
         case 32:
           if ((c1 & 0xFE)==0x30)
             PrintCharAsHex(w);
      }
#endif        
    }
    else
    {
//      PrintChar('.');
    }
  }
  PrintChar(';');
  //TIMSK0 |=1;
  delay(2000);
}


unsigned long ltime0;


//ISR(__vector_1)
void irqP2(void)
{
  unsigned char dat=PORTD;//digitalRead(inpin);
  unsigned short d = 0xffff-T1GetCnt();


  T1SetCnt(0xffff);

  if (dat&4)
    dat=0x80;
  else
    dat=0;
  
  if (d>127)
    dat+=127;
  else
    dat+=(unsigned char)d;
  
  CircularBufPush(in0times,dat);  

}


unsigned char tim;

ISR(TIMER1_CAPT_vect)
{
  unsigned char tccr1b;
  unsigned char dat;
  
//  PORTB |= (1<<PB5);
  
  dat = ICR1L;
  
  if (ICR1H>0)
  {
    dat=127;
    T1SetCnt(4);
  }
  else
  {
    T1SetCnt(T1GetCnt()-dat);
    dat>>=1;
  }
  
  tccr1b=TCCR1B;
  if (!(tccr1b & (1<<ICES1)))
  { //detected rising edge
    dat|=128;
  }
  else
  { //detected falling edge
  }
    //change capture to other edge
  TCCR1B=tccr1b^(1<<ICES1);
  
  CircularBufPush(in0times,dat);
//  PORTB &= ~(1<<PB5);
}


ISR(TIMER1_OVF_vect)
{
//  PORTB |= (1<<PB4);
  //increment overflow counter
  //T1Ovs2++;
//  PORTB &= ~(1<<PB4);
}




ISR(USART_UDRE_vect)
{
  short x;
//  PORTB |= (1<<PB3);  
  x=CircularBufPop(serout);
  
    // if there is data then send it
    if (x>=0)
    {
      UDR0 = x;
    } 
    else
    {
       UCSR0B &= ~(1 << UDRIE0); // no more data to send, turn off data ready interrupt
    }
//  PORTB &= ~(1<<PB3);
}


ISR(xUSART_RX_vect)
{
  
}


ISR(xUSART_TX_vect)
{
  
}


