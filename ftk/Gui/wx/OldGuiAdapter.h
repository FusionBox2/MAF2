#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/IPropertyViewModel.h"

#include "Gui/mafGUI.h"
#include "Gui/mafGUIHolder.h"

#include <memory>
#include <typeindex>
#include <unordered_map>

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	class IOldGuiEditor;
	class OldGuiElement;

	class OldGuiAdapter : public mafBaseEventHandler
	{
	public:
		OldGuiAdapter(mafGUIHolder* panel);

		~OldGuiAdapter() override;

		void setModel(IPropertyViewModel* model);

		template<class T>
		void registerEditor(std::unique_ptr<IOldGuiEditor> editor)
		{
			m_editors[typeid(T)] = std::move(editor);
		}

		void OnEvent(mafEventBase* e) override;

	private:
		void registerDefaultEditors();

		void rebuild();

		void onPropertyChanged();

		//void onPropertyChanged(IPropertyViewModel::PropertyId id);

		//void onGridChanged(wxPropertyGridEvent& event);

		IPropertyViewModel::PropertyId getId(OldGuiElement* prop);

		OldGuiElement* getProp(IPropertyViewModel::PropertyId id);

		mafGUIHolder* m_basePanel = nullptr;
		mafGUI* m_gui = nullptr;
		IPropertyViewModel* m_model = nullptr;
		std::vector<base::Connection> m_connections;
		std::vector<std::unique_ptr<OldGuiElement>> m_elements;
		std::unordered_map<OldGuiElement*, IPropertyViewModel::PropertyId> m_PropToId;
		std::unordered_map<IPropertyViewModel::PropertyId, OldGuiElement*> m_IdToProp;
		std::unordered_map<std::type_index, std::unique_ptr<IOldGuiEditor>> m_editors;
		std::vector<std::tuple<OldGuiElement*, IProperty*, IOldGuiEditor*> > m_bindings;
	};
}

END_FTK_NAMESPACE
