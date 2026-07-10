#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/IPropertyViewModel.h"

#include "ftk/Base/Signal.h"

#include <memory>
#include <vector>

BEGIN_FTK_NAMESPACE

class PropertyViewModel : public gui::IPropertyViewModel
{
public:
	PropertyViewModel();

	~PropertyViewModel() override;

	void setProperties(std::vector<std::unique_ptr<IProperty> > properties);

	size_t propertyCount() const override;

	void changed();

	PropertyId property(size_t index) const override;

	base::Connection connectReset(std::function<void()> fn) override;

	base::Connection connectPropertyChanged(std::function<void()> fn) override;

	//base::Connection connectPropertyChanged(std::function<void(PropertyId)> fn) override;

private:
	base::Signal<> m_reset;
	base::Signal<> m_propertyChanged;
	//base::Signal<PropertyId> m_propertyChanged;
	std::vector<std::unique_ptr<IProperty> > m_properties;
};

END_FTK_NAMESPACE
