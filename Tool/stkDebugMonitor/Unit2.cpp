//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm2::ComboBox0Select(TObject *Sender)
{
  char cContent[60];
  AnsiString as, as1;
  strBigContent* p_strBigContent;
  int para_num;

  p_myini->Read("[COMMAND]", this->ComboBox0->Text.c_str(), cContent);
  CmdString[0] = AnsiString(cContent);
  as = "[" + CmdString[0] + "]";
  p_myini->Read(as.c_str(), "para_num", cContent);
  para_num = atoi(cContent);

  for(int i = 0; i < para_num; i++)
  {
    as1 = "para_" + AnsiString(i) + "_name";
    p_myini->Read(as.c_str(), as1.c_str(), cContent);
    LabelList[i]->Caption = AnsiString(cContent);
    as1 = "para_" + AnsiString(i) + "_mode";
    p_myini->Read(as.c_str(), as1.c_str(), cContent);
    ComboBoxList[i]->Items->Clear();
    ComboBoxList[i]->Text = "";
    if(strcmp("0", cContent) == 0)
    {
      ComboBoxList[i]->Style = csDropDownList;
      as1 = "[" + CmdString[0] + "_" + AnsiString(i) + "]";
      p_myini->ReadFromTitle(as1.c_str(), &p_strBigContent);
      for(int j = 0; j < p_strBigContent->nSmallContentCnt; j++)
      {
        ComboBoxList[i]->Items->Add(AnsiString(p_strBigContent->SmallContent[j].cName));
      }
    }
    else
    {
      ComboBoxList[i]->Style = csSimple;
    }

    ComboBoxList[i]->Enabled = true;
    ComboBoxList[i]->Visible = true;
    LabelList[i]->Visible = true;
  }
  for(int i = para_num; i < CBL_NUM; i++)
  {
    ComboBoxList[i]->Text = "";
    ComboBoxList[i]->Enabled = false;
    ComboBoxList[i]->Visible = false;
    LabelList[i]->Visible = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm2::ComboBox1Select(TObject *Sender)
{
  AnsiString as;
  char cContent[260];
  as = "[" + CmdString[0] + "_0]";
  p_myini->Read(as.c_str(), this->ComboBox1->Text.c_str(), cContent);
  CmdString[1] = AnsiString(cContent);
}
//---------------------------------------------------------------------------

void __fastcall TForm2::ComboBox2Select(TObject *Sender)
{
  AnsiString as;
  char cContent[260];
  as = "[" + CmdString[0] + "_1]";
  p_myini->Read(as.c_str(), this->ComboBox2->Text.c_str(), cContent);
  CmdString[2] = AnsiString(cContent);
}
//---------------------------------------------------------------------------

void __fastcall TForm2::ComboBox3Select(TObject *Sender)
{
  AnsiString as;
  char cContent[260];
  as = "[" + CmdString[0] + "_2]";
  p_myini->Read(as.c_str(), this->ComboBox3->Text.c_str(), cContent);
  CmdString[3] = AnsiString(cContent);
}
//---------------------------------------------------------------------------


AnsiString TForm2::GetCmdString()
{
  AnsiString as = CmdString[0];
  for(int i = 0; i < CBL_NUM; i++)
  {
    if (ComboBoxList[i]->Enabled)
    {
      if (ComboBoxList[i]->Style == csSimple)
      {
        CmdString[i + 1] = ComboBoxList[i]->Text;
      }
      as = as + AnsiString(",") + CmdString[i + 1];
    }
  }
  return as;
}
void __fastcall TForm2::Button1Click(TObject *Sender)
{
  AnsiString as;
  as = GetCmdString();
  frmMain->ComboBox3->Text = as;
  Hide();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::FormCreate(TObject *Sender)
{
  strBigContent* p_strBigContent;

  ComboBoxList[0] = ComboBox1;
  ComboBoxList[1] = ComboBox2;
  ComboBoxList[2] = ComboBox3;
  LabelList[0] = Label1;
  LabelList[1] = Label2;
  LabelList[2] = Label3;
  p_myini = &(frmMain->myini);
  p_myini->ReadFromTitle("[COMMAND]", &p_strBigContent);

  for(int i = 0; i < p_strBigContent->nSmallContentCnt; i++)
  {
    ComboBox0->Items->Add(AnsiString(p_strBigContent->SmallContent[i].cName));
  }
}
//---------------------------------------------------------------------------

