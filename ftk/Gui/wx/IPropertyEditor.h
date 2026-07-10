#pragma once

#include "ftkConfigure.h"

BEGIN_FTK_NAMESPACE

class IProperty;
class wxPGProperty;

namespace gui::wx
{
	class IPropertyEditor
	{
	public:
		virtual ~IPropertyEditor() = default;

		virtual wxPGProperty* create(IProperty* property) = 0;

		virtual void update(wxPGProperty* pgProperty, IProperty* property) = 0;

		virtual void assign(wxPGProperty* pgProperty, IProperty* property) = 0;
	};
}

END_FTK_NAMESPACE
