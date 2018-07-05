//---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
class TCommThread : public TThread
{
  protected:
      void __fastcall Execute(void);
      void __fastcall DisplayUpdate(void);
  public:
      __fastcall TCommThread(void):TThread(true){FreeOnTerminate=true;};
};
//---------------------------------------------------------------------------
#endif
 