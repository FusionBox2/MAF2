#include "ftk/Base/Connection.h"

BEGIN_FTK_NAMESPACE

namespace base
{
	Connection::Connection(std::function<void()>&& disconnect)
		: m_disconnect(std::move(disconnect))
	{
	}

	Connection::~Connection()
	{
		if (m_disconnect)
		{
			m_disconnect();
		}
	}
}

END_FTK_NAMESPACE
