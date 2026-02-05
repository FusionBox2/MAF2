#pragma once

#include "ftkConfigure.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

BEGIN_FTK_NAMESPACE

namespace wxw
{
	const long fbDefaultStyle = wxDEFAULT_FRAME_STYLE;
	wxFrame* CreateFrame(const wxString& title = "", const wxPoint& pos = wxDefaultPosition,
						 const wxSize& size = wxDefaultSize, long style = fbDefaultStyle, const wxString& name = wxFrameNameStr);
}

END_FTK_NAMESPACE
