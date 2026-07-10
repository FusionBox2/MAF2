#include "PropertyAdapter.h"

#include "ftk/Core/IProperty.h"

#include "IPropertyEditor.h"
#include "ftk/Base/mfString.h"

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	PropertyAdapter::PropertyAdapter(wxPropertyGrid* grid)
		: m_grid(grid)
	{
		registerDefaultEditors();

		m_grid->Bind(wxEVT_PG_CHANGED,
			&PropertyAdapter::onGridChanged, this);
	}

	PropertyAdapter::~PropertyAdapter() = default;

	void PropertyAdapter::setModel(IPropertyViewModel* model)
	{
		m_connections.clear();
		m_IdToProp.clear();
		m_PropToId.clear();
		m_grid->Clear();
		m_model = model;

		if (m_model)
		{
			m_connections.push_back(m_model->connectReset([this](){rebuild();}));
			m_connections.push_back(m_model->connectPropertyChanged([this]() {onPropertyChanged(); }));
			//m_connections.push_back(m_model->connectPropertyChanged([this](IPropertyViewModel::PropertyId id){onPropertyChanged(id);}));
			rebuild();
		}
	}

	class StringPropertyEditor : public IPropertyEditor
	{
	public:
		wxPGProperty* create(IProperty* property) override
		{
			return new wxStringProperty(mafStringToWx(property->id()), wxPG_LABEL, std::any_cast<std::string>(property->get()));
		}

		void assign(wxPGProperty* pgProperty, IProperty* property) override
		{
			property->set(std::string(pgProperty->GetValue().GetString().c_str()));
		}

		void update(wxPGProperty* pgProperty, IProperty* property) override
		{
			pgProperty->SetValue(std::any_cast<std::string>(property->get()));
		}
	};

	class UIntPropertyEditor : public IPropertyEditor
	{
	public:
		wxPGProperty* create(IProperty* property) override
		{
			return new wxUIntProperty(mafStringToWx(property->id()), wxPG_LABEL, std::any_cast<uint64_t>(property->get()));
		}

		void assign(wxPGProperty* pgProperty, IProperty* property) override
		{
			property->set(static_cast<uint64_t>(pgProperty->GetValue().GetInteger()));
		}

		void update(wxPGProperty* pgProperty, IProperty* property) override
		{
			pgProperty->SetValue(static_cast<wxULongLong>(std::any_cast<uint64_t>(property->get())));
		}
	};

	void PropertyAdapter::registerDefaultEditors()
	{
		registerEditor<uint64_t>(std::make_unique<UIntPropertyEditor>());
		registerEditor<std::string>(std::make_unique<StringPropertyEditor>());
	}

	void PropertyAdapter::rebuild()
	{
		m_bindings.clear();
		m_grid->Clear();
		m_IdToProp.clear();
		m_PropToId.clear();

		for (size_t i = 0; i < m_model->propertyCount(); ++i)
		{
			auto* property = m_model->property(i);

			if (auto it = m_editors.find(property->type()); it != m_editors.end())
			{
				wxPGProperty* wxProperty = it->second->create(property);
				m_bindings[wxProperty] = std::make_pair(property, it->second.get());

				m_grid->Append(wxProperty);

				m_IdToProp[property] = wxProperty;
				m_PropToId[wxProperty] = property;
			}
		}
	}
	
	void PropertyAdapter::onPropertyChanged()//IPropertyViewModel::PropertyId id)
	{
		for (auto& entry :  m_bindings)
		{
			entry.second.second->update(entry.first, entry.second.first);
		}
	}

	void PropertyAdapter::onGridChanged(wxPropertyGridEvent& event)
	{
		auto wxProperty = event.GetProperty();
		if (auto it = m_bindings.find(wxProperty); it != m_bindings.end())
		{
			it->second.second->assign(wxProperty, it->second.first);
		}
	}

	IPropertyViewModel::PropertyId PropertyAdapter::getId(wxPGProperty* prop)
	{
		if (auto it = m_PropToId.find(prop); it != m_PropToId.end())
		{
			return it->second;
		}
		return nullptr;
	}

	wxPGProperty* PropertyAdapter::getProp(IPropertyViewModel::PropertyId id)
	{
		if (auto it = m_IdToProp.find(id); it != m_IdToProp.end())
		{
			return it->second;
		}
		return nullptr;
	}
}

END_FTK_NAMESPACE
