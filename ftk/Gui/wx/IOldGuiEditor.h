#pragma once

#include "ftkConfigure.h"

BEGIN_FTK_NAMESPACE

class IProperty;
class mafGUI;

namespace gui::wx
{
	class OldGuiElement
	{
	public:
		virtual ~OldGuiElement() = default;
	};

	class IOldGuiEditor
	{
	public:
		virtual ~IOldGuiEditor() = default;

		virtual OldGuiElement* create(mafGUI* gui, int id, IProperty* property) = 0;

		virtual void update(OldGuiElement* element, IProperty* property) = 0;

		virtual void assign(OldGuiElement* element, IProperty* property) = 0;
	};
}

END_FTK_NAMESPACE
