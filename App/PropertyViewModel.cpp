#include "PropertyViewModel.h"

#include "ftk/Core/IProperty.h"

BEGIN_FTK_NAMESPACE

PropertyViewModel::PropertyViewModel() = default;

PropertyViewModel::~PropertyViewModel() = default;

size_t PropertyViewModel::propertyCount() const
{
	return m_properties.size();
}

void PropertyViewModel::setProperties(std::vector<std::unique_ptr<IProperty>> properties)
{
	std::swap(properties, m_properties);
	m_reset.emit();
}

void PropertyViewModel::changed()
{
	m_propertyChanged.emit();
}

gui::IPropertyViewModel::PropertyId PropertyViewModel::property(size_t index) const
{
	return m_properties[index].get();
}

base::Connection PropertyViewModel::connectReset(std::function<void()> fn)
{
	return m_reset.connect(fn);
}

base::Connection PropertyViewModel::connectPropertyChanged(std::function<void()> fn)
{
	return m_propertyChanged.connect(fn);
}

//base::Connection PropertyViewModel::connectPropertyChanged(std::function<void(PropertyId)> fn)
//{
//	return m_propertyChanged.connect(fn);
//}

END_FTK_NAMESPACE
