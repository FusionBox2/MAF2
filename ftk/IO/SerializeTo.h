#pragma once

#include "ftkConfigure.h"

BEGIN_FTK_NAMESPACE

namespace io::serialize
{
	template <class T>
	struct To
	{
		To(){}
		To(T& t)// : m_ref(t)
		{}
		//T& m_ref;
	};
}

END_FTK_NAMESPACE
