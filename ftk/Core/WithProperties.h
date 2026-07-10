#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Signal.h"
#include "ftk/Core/IProperty.h"

#include <memory>
#include <vector>

BEGIN_FTK_NAMESPACE

namespace core
{
	class WithProperties
	{
	public:

		using PropertyList = std::vector<std::unique_ptr<IProperty>>;

		virtual ~WithProperties() = default;

		virtual PropertyList getProperties() = 0;

		base::Connection connectPropertiesChanged(std::function<void()> fn);

	protected:
		base::Signal<> m_propertiesChanged;
	};
}

END_FTK_NAMESPACE
