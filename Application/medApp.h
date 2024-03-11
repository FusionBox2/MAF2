#ifndef __medApp_H__
#define __medApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
class mafLogicWithGUI;
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class medApp : public wxApp
{
public:
  bool OnInit();
  int  OnExit();


  ////Called when the application is in the idle state
  //virtual void OnIdle(wxIdleEvent& event);  
  //DECLARE_EVENT_TABLE()

protected:
  mafLogicWithGUI *m_Logic;
};
DECLARE_APP(medApp)
#endif 
