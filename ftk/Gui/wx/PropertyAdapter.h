#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/IPropertyViewModel.h"

#include "wx/propgrid/propgrid.h"

#include <memory>
#include <typeindex>
#include <unordered_map>

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	class IPropertyEditor;

	class PropertyAdapter
	{
	public:
		PropertyAdapter(wxPropertyGrid* grid);

		~PropertyAdapter();

		void setModel(IPropertyViewModel* model);

		template<class T>
		void registerEditor(std::unique_ptr<IPropertyEditor> editor)
		{
			m_editors[typeid(T)] = std::move(editor);
		}

	private:
		void registerDefaultEditors();

		void rebuild();

		void onPropertyChanged();

		//void onPropertyChanged(IPropertyViewModel::PropertyId id);

		void onGridChanged(wxPropertyGridEvent& event);

		IPropertyViewModel::PropertyId getId(wxPGProperty* prop);

		wxPGProperty* getProp(IPropertyViewModel::PropertyId id);

		wxPropertyGrid* m_grid;
		IPropertyViewModel* m_model = nullptr;
		std::vector<base::Connection> m_connections;
		std::unordered_map<wxPGProperty*, IPropertyViewModel::PropertyId> m_PropToId;
		std::unordered_map<IPropertyViewModel::PropertyId, wxPGProperty*> m_IdToProp;
		std::unordered_map<std::type_index, std::unique_ptr<IPropertyEditor>> m_editors;
		std::unordered_map<wxPGProperty*, std::pair<IProperty*, IPropertyEditor*> > m_bindings;
	};
}

END_FTK_NAMESPACE
