//---------------------------------------------------------------------------
#pragma hdrstop

#include "Unit3.h"
#include "Unit1.h"
#include "PcommApi.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

#define DBG_INFO_MAX_SIZE 15360
char dgb_info_buf[DBG_INFO_MAX_SIZE];
int dbg_info_len;
__int64 total_rx_cnt;
int file_cnt;
const char chat_tab[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

void __fastcall TCommThread::DisplayUpdate(void)
{
  frmMain->DisplayUpdate(total_rx_cnt, dgb_info_buf, dbg_info_len);
}

void __fastcall TCommThread::Execute(void)
{
  char t_rx_buf[2046],t_dgb_info_buf[DBG_INFO_MAX_SIZE],t_swap_buf[DBG_INFO_MAX_SIZE];
  int t_rx_cnt;
  unsigned char t_chksum;
  char *t_fp,*t_p;
  bool t_flag_err_time_valid;

  FlushInBuffer(com_port.Port);
  total_rx_cnt = 0;
  file_cnt = 0;
  memset(t_dgb_info_buf, 0, DBG_INFO_MAX_SIZE);

  while(!this->Terminated)
  {
    if (flag_switch_done)
    {
      if (flag_debug)
      {
        tx_cnt = strlen("TDCMD,1");
        memcpy(tx_buf,"TDCMD,1",tx_cnt);
      }
      else
      {
        tx_cnt = strlen("TDCMD,0");
        memcpy(tx_buf,"TDCMD,0",tx_cnt);
      }
    }
    if(tx_cnt)
    {
      t_chksum = 0;
      for(int i = 0; i < tx_cnt; i++)
      {
        t_chksum = t_chksum ^ tx_buf[i];
      }
      tx_buf[tx_cnt] = '*';
      tx_buf[tx_cnt + 1] = chat_tab[t_chksum >> 4];
      tx_buf[tx_cnt + 2] = chat_tab[t_chksum & 0xF];
      tx_buf[tx_cnt + 3] = '\r';
      tx_buf[tx_cnt + 4] = '\n';

      WriteCOM(com_port.Port,"$",1);
      WriteCOM(com_port.Port,tx_buf,tx_cnt + 5);
      tx_cnt = 0;
    }

    memset(t_rx_buf, 0, 2046);

    t_rx_cnt = ReadCOM(com_port.Port,t_rx_buf,2046);
    
    if(t_rx_cnt > 0)
    {
      if(flag_capture)
      {
        fwrite(t_rx_buf, t_rx_cnt, 1, file_log.Hdl);
        file_cnt = file_cnt + t_rx_cnt;
        if (file_cnt >= 50*1024*1024)
        {
          fclose(file_log.Hdl);
          file_log.Name = ".\\capture\\" + file_log.COMName +FormatDateTime("_YYYYMMDD_HHMMSS",Now()) + ".txt";
          file_log.Hdl = fopen(file_log.Name.c_str(),"wb");
          file_cnt = 0;
        }
      }

      total_rx_cnt = total_rx_cnt + t_rx_cnt;

      if(strlen(t_dgb_info_buf) > (DBG_INFO_MAX_SIZE - 2046))
      {
        memcpy(t_dgb_info_buf,t_dgb_info_buf + strlen(t_dgb_info_buf) - 16 ,16);
        memset(&t_dgb_info_buf[16], 0, DBG_INFO_MAX_SIZE - 16);
      }
      strcat(t_dgb_info_buf, t_rx_buf);
      memset(dgb_info_buf,0,DBG_INFO_MAX_SIZE);
      dbg_info_len = 0;
      
      if(flag_block_check)
      {
        t_fp = strstr(&t_dgb_info_buf[1],block_check_buf);
        if(t_fp)
        {
          dbg_info_len = t_fp - t_dgb_info_buf;
          memcpy(dgb_info_buf,t_dgb_info_buf,dbg_info_len);
          memset(t_swap_buf, 0, DBG_INFO_MAX_SIZE);
          memcpy(t_swap_buf,t_fp,strlen(t_fp));
          memcpy(t_dgb_info_buf,t_swap_buf,DBG_INFO_MAX_SIZE);
        }
      }
      else
      {
         t_p = t_dgb_info_buf;
         t_fp = strstr(t_p,"\r\n");
         while(t_fp)
         {
           t_p = t_fp + 2;
           t_fp = strstr(t_p,"\r\n");
         }
         if(t_p != t_dgb_info_buf)
         {
           dbg_info_len = t_p - t_dgb_info_buf;
           memcpy(dgb_info_buf,t_dgb_info_buf,dbg_info_len);
           memset(t_swap_buf, 0, DBG_INFO_MAX_SIZE);
           memcpy(t_swap_buf,t_p,strlen(t_p));
           memcpy(t_dgb_info_buf,t_swap_buf,DBG_INFO_MAX_SIZE);
           dbg_info_len = dbg_info_len - 2;
         }
      }
      if (flag_switch_done)
      {
        if (flag_debug)
        {
          ClosePort(&com_port);
          com_port.BaudRate = "115200";
          InitPort(&com_port);
        }
        else
        {
          ClosePort(&com_port);
          com_port.BaudRate = "9600";
          InitPort(&com_port);
        }
      }
      Synchronize(DisplayUpdate);
      flag_switch_done = false;
    }

    Sleep(50);
    Application->ProcessMessages();
  }
}
