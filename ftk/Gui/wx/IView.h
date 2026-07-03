#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/IViewModel.h"

#include <wx/wx.h>

#include <memory>

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	class IView
	{
	public:
		virtual ~IView();

		virtual wxWindow* widget() = 0;

		virtual void attach(std::unique_ptr<IViewModel> viewModel) = 0;

		virtual void detach() = 0;

		virtual void setActive(bool active) = 0;
	};
}

END_FTK_NAMESPACE
