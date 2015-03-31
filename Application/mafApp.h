#ifndef __mafApp_H__
#define __mafApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
class mafLogicWithGUI;
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafApp : public wxApp
{
public:
  bool OnInit();
  int  OnExit();
protected:
  mafLogicWithGUI *m_Logic;
};
DECLARE_APP(mafApp)
#endif 
