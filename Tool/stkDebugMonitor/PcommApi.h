#ifndef LT_FUN_COMH
#define LT_FUN_COMH

//------------------------------------------------------------------------------

#include <vcl.h>
#include "pcomm.h"

//------------------------------------------------------------------------------

typedef struct COMPORT_STU
{
  int Port;
  AnsiString BaudRate;
  AnsiString DataLenght;
  AnsiString StopBit;
  AnsiString Parity;
  bool Opened;
}TCOMPort;
typedef DynamicArray<TCOMPort> TCOMPortDynArray;

//-----------------------------------------------------------------------

int __stdcall ConvertBaud(char *baud)
{
 if(strcmp(baud,"50")==0) return B50;
 if(strcmp(baud,"600")==0) return B600;
 if(strcmp(baud,"9600")==0) return B9600;
 if(strcmp(baud,"460800")==0) return B460800;
 if(strcmp(baud,"75")==0) return B75;
 if(strcmp(baud,"1200")==0) return B1200;
 if(strcmp(baud,"19200")==0) return B19200;
 if(strcmp(baud,"921600")==0) return B921600;
 if(strcmp(baud,"110")==0) return B110;
 if(strcmp(baud,"1800")==0) return B1800;
 if(strcmp(baud,"38400")==0) return B38400;
 if(strcmp(baud,"134.5")==0) return B134;
 if(strcmp(baud,"2400")==0) return B2400;
 if(strcmp(baud,"57600")==0) return B57600;
 if(strcmp(baud,"150")==0) return B150;
 if(strcmp(baud,"4800")==0) return B4800;
 if(strcmp(baud,"115200")==0) return B115200;
 if(strcmp(baud,"300")==0) return B300;
 if(strcmp(baud,"7200")==0) return B7200;
 if(strcmp(baud,"230400")==0) return B230400;
 return B9600;
}
//-------------------------------------

int __stdcall ConvertMode(char *BitCnt,char *Parity,char *StopBit)
{
 int bit_cnt,parity,stop_bit;

 if(strcmp(BitCnt,"5")==0) bit_cnt=BIT_5;
 else
  if(strcmp(BitCnt,"6")==0) bit_cnt=BIT_6;
  else
   if(strcmp(BitCnt,"7")==0) bit_cnt=BIT_7;
   else
    if(strcmp(BitCnt,"8")==0) bit_cnt=BIT_8;
    else   bit_cnt=BIT_8;

 if(strcmp(Parity,"ÎÞ")==0) parity=P_NONE;
 else
  if(strcmp(Parity,"Å¼")==0) parity=P_EVEN;
  else
   if(strcmp(Parity,"Ææ")==0) parity=P_ODD;
   else
    if(strcmp(Parity,"±ê¼Ç")==0) parity=P_MRK;
    else
     if(strcmp(Parity,"¿Õ")==0) parity=P_SPC;
     else parity=P_NONE;

 if(strcmp(StopBit,"1")==0) stop_bit=STOP_1;
 else if(strcmp(StopBit,"2")==0) stop_bit=STOP_2;
      else stop_bit=STOP_1;

 return (bit_cnt|parity|stop_bit);
}
//-------------------------------------------

bool __stdcall ClosePort(TCOMPort *COMPort)
{
 if(!COMPort->Opened) return 1;
 if(sio_close(COMPort->Port)!=SIO_OK) return 0;
 COMPort->Opened=0;
 return 1;
}
//--------------------------------------------

bool __stdcall InitPort(TCOMPort *COMPort)
{
 COMPort->Opened=0;
 if(sio_open(COMPort->Port)!=SIO_OK) return 0;
 else  if(sio_ioctl(COMPort->Port,ConvertBaud(COMPort->BaudRate.c_str()),ConvertMode(COMPort->DataLenght.c_str(),COMPort->Parity.c_str(),COMPort->StopBit.c_str()))!=SIO_OK)  return 0;
 COMPort->Opened=1;
 return 1;
}
//--------------------------------------------

int __stdcall SetTermInt(int Port,VOID (CALLBACK *func)(int Port),char Code)
{
 return sio_term_irq(Port,func,Code);
}
//--------------------------------------------

int __stdcall SetCntInt(int Port,VOID (CALLBACK *func)(int Port),int Count)
{
 return sio_cnt_irq(Port,func,Count);
}
//--------------------------------------------

int __stdcall ReadCOM(int Port, char *Buf, int Len)
{
 return sio_read(Port,Buf,Len);
}
//--------------------------------------------

int __stdcall WriteCOM(int Port, char *Buf, int Len)
{
 return sio_write(Port,Buf,Len);
}
//--------------------------------------------

int __stdcall FlushOutBuffer(int Port)
{
 return sio_flush(Port,1);
}
//--------------------------------------------

int __stdcall FlushInBuffer(int Port)
{
 return sio_flush(Port,0);
}
//------------------------------------------------------------------------------

int __stdcall BufCount(int Port)
{
 return sio_iqueue(Port);
}
//------------------------------------------------------------------------------
#endif