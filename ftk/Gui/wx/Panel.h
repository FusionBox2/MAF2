#pragma once

#include "ftkConfigure.h"

#ifdef MAF_USE_WX
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#endif

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	using Panel = wxPanel;
}

using mafGUIPanel = gui::wx::Panel;

END_FTK_NAMESPACE
