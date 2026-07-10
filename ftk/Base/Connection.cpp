#include "ftk/Base/Connection.h"

BEGIN_FTK_NAMESPACE

namespace base
{
	Connection::Connection(std::function<void()>&& disconnect)
		: m_disconnect(std::move(disconnect))
	{
	}

	Connection::Connection(Connection&& connection)
		: m_disconnect(std::move(connection.m_disconnect))
	{
	}

	Connection& Connection::operator=(Connection&& connection)
	{
		std::swap(m_disconnect, connection.m_disconnect);
		return *this;
	}

	Connection::~Connection()
	{
		if (m_disconnect)
		{
			m_disconnect();
		}
	}

	void Connection::disconnect()
	{
		if (m_disconnect)
		{
			auto disconnect(std::move(m_disconnect));
			disconnect();
		}
	}
}

END_FTK_NAMESPACE
