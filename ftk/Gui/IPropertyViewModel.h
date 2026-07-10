#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/IViewModel.h"

#include "ftk/Base/Connection.h"

BEGIN_FTK_NAMESPACE

class IProperty;

namespace gui
{
	class IPropertyViewModel : public IViewModel
	{
	public:
		using PropertyId = IProperty*;

		~IPropertyViewModel() override = default;

		virtual size_t propertyCount() const = 0;

		virtual IProperty* property(size_t index) const = 0;

		virtual base::Connection connectReset(std::function<void()> fn) = 0;

		virtual base::Connection connectPropertyChanged(std::function<void()> fn) = 0;

		//virtual base::Connection connectPropertyChanged(std::function<void(PropertyId)> fn) = 0;
	};
}

END_FTK_NAMESPACE
