//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "dir.h"
#include "dirent.h"
#include "Unit2.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;

//---------------------------------------------------------------------------
void Delay(DWORD _to)
{
  DWORD t;
  t = GetTickCount();
  while((GetTickCount() - t) > _to);
}

void GetSerialComm(TStrings *_item)
{
  #define BUF_LEN 64
  HKEY hKey;
  DWORD ulValues;
  TCHAR cValueName[BUF_LEN];
  DWORD ulValueNameLen = BUF_LEN;
  TCHAR cData[BUF_LEN];
  DWORD ulDataLen = BUF_LEN;

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\SERIALCOMM",0,KEY_READ,&hKey) != ERROR_SUCCESS)
  {
    return;
  }

  if(RegQueryInfoKey(hKey,NULL,NULL,NULL,NULL,NULL,NULL,&ulValues,NULL,NULL,NULL,NULL) != ERROR_SUCCESS)
  {
    return;
  }

  for(DWORD i=0; i < ulValues; i++)
  {
    ulValueNameLen = BUF_LEN;
    ulDataLen = BUF_LEN;
    if(RegEnumValue(hKey,i,cValueName,&ulValueNameLen,NULL,NULL,cData,&ulDataLen)!= ERROR_SUCCESS)
    {
        continue;
    }
    _item->Add(AnsiString(cData));
  }
}
//---------------------------------------------------------------------------
TCommThread *comm_thread;

TCOMPort com_port;
TFileClass file_log;
bool flag_capture = false;
bool flag_timer = false;
bool flag_debug = false;
bool flag_switch_done = false;

char tx_buf[64];
int tx_cnt;
bool flag_block_check;
char block_check_buf[16];
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
        : TForm(Owner)
{
  myini.Open(".\\DebugMonitor.ini");
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormShow(TObject *Sender)
{
  this->DoubleBuffered = true;
  this->PageControl1->DoubleBuffered = true;
  this->TabSheet1->DoubleBuffered =true;
  this->TabSheet3->DoubleBuffered =true;
  this->Memo1->DoubleBuffered = true;
  this->Memo2->DoubleBuffered = true;
  this->CheckBox1->Checked = false;
  this->Edit1->Enabled = false;
  GetSerialComm(this->ComboBox1->Items);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::DisplayUpdate(int _total_rx_cnt, char *_dgb_info_buf, int _len)
{
  this->StatusBar1->Panels->Items[1]->Text = IntToStr(_total_rx_cnt);
  if (flag_switch_done)
  {
    if (flag_debug)
    {
      this->ComboBox2->ItemIndex = 0;
      this->ComboBox4->ItemIndex = 0;
      flag_block_check = true;
      memset(block_check_buf, 0, 16);
      memcpy(block_check_buf, this->ComboBox4->Text.c_str(), strlen(this->ComboBox4->Text.c_str()));
      this->PageControl1->ActivePageIndex = 0;
      this->Button6->Caption = "NMEA";
    }
    else
    {
      this->ComboBox2->ItemIndex = 4;
      flag_block_check = false;
      this->PageControl1->ActivePageIndex = 1;
      this->Button6->Caption = "DEBUG";
    }
  }
  if(_len == 0)
  {
    return;
  }

  if(flag_block_check)
  {
    this->StatusBar1->Panels->Items[3]->Text = IntToStr(_len);
    this->Memo1->Text = AnsiString(_dgb_info_buf, _len);

    if(this->Memo2->Lines->Count > 50000)
    {
      this->Memo2->Clear();
    }
    this->Memo2->Lines->Add(this->Memo1->Text);
  }
  else
  {
    this->StatusBar1->Panels->Items[3]->Text = "0";
    this->Memo1->Clear();
    if(this->Memo2->Lines->Count > 50000)
    {
      this->Memo2->Clear();
    }
    this->Memo2->Lines->Add(AnsiString(_dgb_info_buf, _len));
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Button1Click(TObject *Sender)
{
  if(this->ComboBox1->ItemIndex == -1)
  {
    return;
  }
  
  char *com_buf = this->ComboBox1->Text.c_str();
  com_buf += 3;
  com_port.Port = atoi(com_buf);
  com_port.BaudRate = this->ComboBox2->Text;
  com_port.DataLenght = "8";
  com_port.StopBit = "1";
  com_port.Parity = "无";
  com_port.Opened = false;

  if(!InitPort(&com_port))
  {
    ShowMessage("打开串口错误！");
    return;
  }

  this->ComboBox1->Enabled = false;
  this->ComboBox2->Enabled = false;
  this->ComboBox3->Enabled = true;
  this->ComboBox4->Enabled = false;

  this->Button1->Enabled = false;
  this->Button2->Enabled = true;
  this->Button3->Enabled = true;
  this->Button4->Enabled = true;
  this->Button6->Enabled = true;
  this->Button7->Enabled = true;

  if(this->ComboBox4->Text.IsEmpty())
  {
    flag_block_check = false;
    this->PageControl1->ActivePageIndex = 1;
  }
  else
  {
    flag_block_check = true;
    memset(block_check_buf, 0, 16);
    memcpy(block_check_buf, this->ComboBox4->Text.c_str(), strlen(this->ComboBox4->Text.c_str()));
    this->PageControl1->ActivePageIndex = 0;
  }

  comm_thread = new TCommThread();
  comm_thread->Resume();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Button2Click(TObject *Sender)
{
  if(flag_capture)
  {
    if( Application->MessageBoxA("正在抓取串口数据到文件中，是否关闭串口？","确认",MB_YESNO) == IDNO)
    {
      return;
    }
    this->Button3Click(Sender);
  }
  comm_thread->Terminate();
  comm_thread = NULL;

  Delay(1000);

  ClosePort(&com_port);

  this->ComboBox1->Enabled = true;
  this->ComboBox2->Enabled = true;
  this->ComboBox3->Enabled = false;
  this->ComboBox4->Enabled = true;
  
  this->Button1->Enabled = true;
  this->Button2->Enabled = false;
  this->Button3->Enabled = false;
  this->Button4->Enabled = false;
  this->Button6->Enabled = false;
  this->Button7->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  if(com_port.Opened)
  {
    if( Application->MessageBoxA("正在捕获串口数据，是否退出？","确认",MB_YESNO) == IDNO)
    {
      CanClose = false;
      return;
    }
    
    this->Button2Click(Sender);
  }
  CanClose = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Button3Click(TObject *Sender)
{
  if(!flag_capture)
  {
    if (opendir(".\\capture") == NULL)
    {
        mkdir(".\\capture");
    }
    file_log.COMName = this->ComboBox1->Text;
    file_log.Name = ".\\capture\\" + file_log.COMName +FormatDateTime("_YYYYMMDD_HHMMSS",Now()) + ".txt";
    file_log.Hdl = fopen(file_log.Name.c_str(),"wb");
    if(file_log.Hdl)
    {
      flag_capture = true;
      this->Button3->Caption = "Stop Capture";
    }
  }
  else
  {
    fclose(file_log.Hdl);
    flag_capture = false;
    this->Button3->Caption = "Capture Text";
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Button5Click(TObject *Sender)
{
  this->Memo1->Clear();
  this->Memo2->Clear();
  this->StatusBar1->Panels->Items[1]->Text = "0";
  this->StatusBar1->Panels->Items[3]->Text = "0";
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Button4Click(TObject *Sender)
{
  if(this->ComboBox3->Text.IsEmpty())
  {
    return;
  }

  if (flag_timer)
  {
     this->Timer1->Interval = this->Edit1->Text.ToInt();
     this->Timer1->Enabled = true;
  }
  else
  {
    tx_cnt = strlen(this->ComboBox3->Text.c_str());
    memcpy(tx_buf,this->ComboBox3->Text.c_str(),tx_cnt);
  }
  if (ComboBox3->Items->IndexOf(ComboBox3->Text) == -1)
  {
    ComboBox3->Items->Add(ComboBox3->Text);
  }
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::CheckBox1Click(TObject *Sender)
{
   if (flag_timer)
   {
      flag_timer = false;
      this->Timer1->Enabled = false;
      this->Edit1->Enabled = false;
   }
   else
   {
      flag_timer = true;
      this->Edit1->Enabled = true;
   }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Timer1Timer(TObject *Sender)
{
  tx_cnt = strlen(this->ComboBox3->Text.c_str());
  memcpy(tx_buf,this->ComboBox3->Text.c_str(),tx_cnt);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Button6Click(TObject *Sender)
{
  flag_debug = !flag_debug;
  flag_switch_done = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Button7Click(TObject *Sender)
{
  Form2->Show();
}
//---------------------------------------------------------------------------

