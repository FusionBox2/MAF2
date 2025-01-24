#ifndef _FTK_MAINWND_H_
#define _FTK_MAINWND_H_

#include <wx/wx.h>
//#include "Gui/ftkGuiConfigure.h"
//#include "Gui/ftkGuiConfigure.h"
#define MAF_FTKGUI_EXPORT
namespace ftk
{
  typedef wxMDIParentFrame MainWndBase;
  const long fbDefaultStyle = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL;
  //typedef wxFrame FusionBoxMainWndBase;
  //const long fbDefaultStyle = wxDEFAULT_FRAME_STYLE;
  //typedef wxAuiMDIParentFrame FusionBoxMainWndBase;

  class MAF_FTKGUI_EXPORT MainWnd : public MainWndBase
  {
  public:
    MainWnd();
    MainWnd(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize, long style = fbDefaultStyle, const wxString& name = wxFrameNameStr);
    virtual ~MainWnd();

    bool Create(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize, long style = fbDefaultStyle, const wxString& name = wxFrameNameStr);
    //void Configure();
    //bool CreateToolBar();

  protected:
  private:
    wxDECLARE_DYNAMIC_CLASS(MainWnd);

    wxDECLARE_EVENT_TABLE();
  };
  //const long fbDefaultStyle = wxDEFAULT_FRAME_STYLE;
  wxFrame* CreateFrame(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY, const wxString& title = "", const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize, long style = fbDefaultStyle, const wxString& name = wxFrameNameStr);
}//namespace ftk
#endif
