#pragma once

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

namespace ftk
{
    const long fbDefaultStyle = wxDEFAULT_FRAME_STYLE;
    wxFrame* CreateFrame(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY, const wxString& title = "", const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize, long style = fbDefaultStyle, const wxString& name = wxFrameNameStr);
}
