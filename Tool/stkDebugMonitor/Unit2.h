//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Myini.h"
#define CBL_NUM 3
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label0;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TButton *Button1;
        TComboBox *ComboBox0;
        TComboBox *ComboBox1;
        TComboBox *ComboBox2;
        TComboBox *ComboBox3;
        void __fastcall ComboBox0Select(TObject *Sender);
        void __fastcall ComboBox1Select(TObject *Sender);
        void __fastcall ComboBox2Select(TObject *Sender);
        void __fastcall ComboBox3Select(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
        TComboBox *ComboBoxList[CBL_NUM];
        TLabel *LabelList[CBL_NUM];
public:		// User declarations
        CMyini* p_myini;
        AnsiString CmdString[CBL_NUM + 1];
        __fastcall TForm2(TComponent* Owner);
        AnsiString GetCmdString();
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------
#endif
