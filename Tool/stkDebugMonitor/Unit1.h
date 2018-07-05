//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>

#include <stdio.h>
#include <SyncObjs.hpp>

#include "Unit3.h"

#include "PcommApi.h"
#include <Chart.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include "Myini.h"
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TLabel *Label1;
        TComboBox *ComboBox1;
        TButton *Button2;
        TComboBox *ComboBox2;
        TButton *Button1;
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet3;
        TStatusBar *StatusBar1;
        TMemo *Memo2;
        TMemo *Memo1;
	TComboBox *ComboBox3;
	TButton *Button4;
	TButton *Button3;
	TButton *Button5;
	TLabel *Label2;
	TComboBox *ComboBox4;
	TLabel *Label3;
        TLabel *Label4;
        TEdit *Edit1;
        TLabel *Label5;
        TCheckBox *CheckBox1;
        TTimer *Timer1;
        TButton *Button6;
        TButton *Button7;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall Button5Click(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
        void __fastcall CheckBox1Click(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall Button6Click(TObject *Sender);
        void __fastcall Button7Click(TObject *Sender);
private:	// User declarations

public:		// User declarations
        void __fastcall DisplayUpdate(int _total_rx_cnt, char *_dgb_info_buf, int _len);
        __fastcall TfrmMain(TComponent* Owner);
        CMyini myini;
};

extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
typedef struct
{
  FILE* Hdl;
  AnsiString Name;
  AnsiString COMName;
}TFileClass;

extern TCOMPort com_port;
extern TFileClass file_log;
extern bool flag_capture;
extern char tx_buf[];
extern int tx_cnt;
extern bool flag_block_check;
extern char block_check_buf[];
extern bool flag_debug;
extern bool flag_switch_done;
//---------------------------------------------------------------------------
#endif
