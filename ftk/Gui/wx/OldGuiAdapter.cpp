#include "OldGuiAdapter.h"

#include "ftk/Core/IProperty.h"

#include "IOldGuiEditor.h"

namespace
{
	class StringOldGuiElement : public gui::wx::OldGuiElement
	{
	public:
		mafString m_value;
	};

	class StringOldGuiEditor : public gui::wx::IOldGuiEditor
	{
	public:
		StringOldGuiElement* create(mafGUI* gui, int id, IProperty* property) override
		{
			base::String label = property->id();
			if (auto descr = property->metadata().get<base::String>(_R("visual_name")); descr)
			{
				label = *descr;
			}

			auto typedElement = new StringOldGuiElement;
			typedElement->m_value = mafStdToString(std::any_cast<std::string>(property->get()));
			gui->String(id, label, &typedElement->m_value);
			return typedElement;
		}

		void assign(gui::wx::OldGuiElement* element, IProperty* property) override
		{
			auto typedElement = static_cast<StringOldGuiElement*>(element);
			property->set(mafStringToStd(typedElement->m_value));
		}

		void update(gui::wx::OldGuiElement* element, IProperty* property) override
		{
			auto typedElement = static_cast<StringOldGuiElement*>(element);
			typedElement->m_value = mafStdToString(std::any_cast<std::string>(property->get()));
		}
	};

	class IntOldGuiElement : public gui::wx::OldGuiElement
	{
	public:
		int m_value;
	};

	template<typename T>
	class IntOldGuiEditor : public gui::wx::IOldGuiEditor
	{
	public:
		IntOldGuiElement* create(mafGUI* gui, int id, IProperty* property) override
		{
			auto typedElement = new IntOldGuiElement;
			typedElement->m_value = std::any_cast<T>(property->get());
			base::String label = property->id();
			if (auto descr = property->metadata().get<base::String>(_R("visual_name")); descr)
			{
				label = *descr;
			}

			if (auto enumType = property->metadata().get<base::String>(_R("enum_type")); enumType)
			{
				if (auto descriptions = property->metadata().get<std::vector<base::String>>(_R("entries")); descriptions)
				{
					if (*enumType == _R("combo"))
					{
						gui->Combo(id, label, &typedElement->m_value, descriptions->size(), descriptions->data());
					}
					else //if (*enumType == _R("radio"))
					{
						gui->Radio(id, label, &typedElement->m_value, descriptions->size(), descriptions->data());
					}
				}
			}
			else if (auto boolFlag = property->metadata().get<bool>(_R("boolean")); boolFlag && *boolFlag)
			{
				gui->Bool(id, label, &typedElement->m_value);
			}
			else
			{
				gui->Integer(id, label, &typedElement->m_value);
			}

			return typedElement;
		}

		void assign(gui::wx::OldGuiElement* element, IProperty* property) override
		{
			auto typedElement = static_cast<IntOldGuiElement*>(element);
			property->set(static_cast<T>(typedElement->m_value));
		}

		void update(gui::wx::OldGuiElement* element, IProperty* property) override
		{
			auto typedElement = static_cast<IntOldGuiElement*>(element);
			typedElement->m_value = std::any_cast<T>(property->get());
		}
	};

	class FloatOldGuiElement : public gui::wx::OldGuiElement
	{
	public:
		double m_value;
	};

	template<typename T>
	class FloatOldGuiEditor : public gui::wx::IOldGuiEditor
	{
	public:
		FloatOldGuiElement* create(mafGUI* gui, int id, IProperty* property) override
		{
			auto typedElement = new FloatOldGuiElement;
			typedElement->m_value = std::any_cast<T>(property->get());
			base::String label = property->id();

			if (auto descr = property->metadata().get<base::String>(_R("visual_name")); descr)
			{
				label = *descr;
			}
			gui->Double(id, label, &typedElement->m_value);
			return typedElement;
		}

		void assign(gui::wx::OldGuiElement* element, IProperty* property) override
		{
			auto typedElement = static_cast<FloatOldGuiElement*>(element);
			property->set(static_cast<T>(typedElement->m_value));
		}

		void update(gui::wx::OldGuiElement* element, IProperty* property) override
		{
			auto typedElement = static_cast<FloatOldGuiElement*>(element);
			typedElement->m_value = std::any_cast<T>(property->get());
		}
	};

	class BooleanOldGuiElement : public gui::wx::OldGuiElement
	{
	public:
		int m_value;
	};

	class BooleanOldGuiEditor : public gui::wx::IOldGuiEditor
	{
	public:
		BooleanOldGuiElement* create(mafGUI* gui, int id, IProperty* property) override
		{
			auto typedElement = new BooleanOldGuiElement;
			typedElement->m_value = std::any_cast<bool>(property->get());
			base::String label = property->id();

			if (auto descr = property->metadata().get<base::String>(_R("visual_name")); descr)
			{
				label = *descr;
			}
			gui->Bool(id, label, &typedElement->m_value);
			return typedElement;
		}

		void assign(gui::wx::OldGuiElement* element, IProperty* property) override
		{
			auto typedElement = static_cast<BooleanOldGuiElement*>(element);
			property->set(static_cast<bool>(typedElement->m_value));
		}

		void update(gui::wx::OldGuiElement* element, IProperty* property) override
		{
			auto stringElement = static_cast<StringOldGuiElement*>(element);
			stringElement->m_value = mafStdToString(std::any_cast<std::string>(property->get()));
		}
	};
}

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	OldGuiAdapter::OldGuiAdapter(mafGUIHolder* panel)
		: m_basePanel(panel)
	{
		registerDefaultEditors();
	}

	OldGuiAdapter::~OldGuiAdapter() = default;

	void OldGuiAdapter::setModel(IPropertyViewModel* model)
	{
		if (m_gui)
		{
			m_basePanel->RemoveCurrentGui();
			delete m_gui;
		}
		m_connections.clear();
		m_IdToProp.clear();
		m_PropToId.clear();
		m_model = model;

		if (m_model)
		{
			m_connections.push_back(m_model->connectReset([this](){rebuild();}));
			m_connections.push_back(m_model->connectPropertyChanged([this]() {onPropertyChanged(); }));
			//m_connections.push_back(m_model->connectPropertyChanged([this](IPropertyViewModel::PropertyId id){onPropertyChanged(id);}));
			rebuild();
		}
	}

	void OldGuiAdapter::OnEvent(mafEventBase* e)
	{
		auto event = static_cast<mafEvent*>(e);
		auto& entry = m_bindings[event->GetId() - 10000];
		std::get<2>(entry)->assign(std::get<0>(entry), std::get<1>(entry));
	}

	void OldGuiAdapter::registerDefaultEditors()
	{
		registerEditor<double>(std::make_unique<FloatOldGuiEditor<double>>());
		registerEditor<int>(std::make_unique<IntOldGuiEditor<int>>());
		registerEditor<std::string>(std::make_unique<StringOldGuiEditor>());
	}

	void OldGuiAdapter::rebuild()
	{
		auto bindings = std::move(m_bindings);
		auto IdToProp = std::move(m_IdToProp);
		auto PropToId = std::move(m_PropToId);
		auto elements = std::move(m_elements);
		mafGUI* oldgui = nullptr;
		m_bindings.clear();
		m_IdToProp.clear();
		m_PropToId.clear();
		m_elements.clear();
		if (m_gui)
		{
			m_gui->SetListener(nullptr);
			m_basePanel->RemoveCurrentGui();
			oldgui = m_gui;
		}

		auto f = mafGetFrame();
		mafSetFrame(m_basePanel);
		m_gui = new mafGUI(this);
		mafSetFrame(f);

		for (size_t i = 0; i < m_model->propertyCount(); ++i)
		{
			auto* property = m_model->property(i);
			base::String label = property->id();

			if (auto it = m_editors.find(property->type()); it != m_editors.end())
			{
				int newId = m_bindings.size() + 10000;
				std::unique_ptr<OldGuiElement> element(it->second->create(m_gui, newId, property));
				m_bindings.emplace_back(element.get(), property, it->second.get());
				m_IdToProp[property] = element.get();
				m_PropToId[element.get()] = property;
				m_elements.push_back(std::move(element));
				if (auto enabled = property->metadata().get<std::function<bool()>>(_R("enabled")); enabled)
				{
					m_gui->Enable(newId, (*enabled)());
				}
			}
		}
		m_basePanel->Put(m_gui);
		m_gui->Update();
		delete oldgui;
	}
	
	void OldGuiAdapter::onPropertyChanged()//IPropertyViewModel::PropertyId id)
	{
		for (size_t i = 0; i < m_bindings.size(); i++)
		{
			auto& entry = m_bindings[i];
			std::get<2>(entry)->update(std::get<0>(entry), std::get<1>(entry));
			if (auto enabled = std::get<1>(entry)->metadata().get<std::function<bool()>>(_R("enabled")); enabled)
			{
				m_gui->Enable(10000 + i, (*enabled)());
			}
		}
		m_gui->Update();
	}

	IPropertyViewModel::PropertyId OldGuiAdapter::getId(OldGuiElement* prop)
	{
		if (auto it = m_PropToId.find(prop); it != m_PropToId.end())
		{
			return it->second;
		}
		return nullptr;
	}

	OldGuiElement* OldGuiAdapter::getProp(IPropertyViewModel::PropertyId id)
	{
		if (auto it = m_IdToProp.find(id); it != m_IdToProp.end())
		{
			return it->second;
		}
		return nullptr;
	}
}

END_FTK_NAMESPACE
