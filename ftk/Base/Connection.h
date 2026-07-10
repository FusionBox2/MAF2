#pragma once

#include "ftkConfigure.h"

#include <functional>

BEGIN_FTK_NAMESPACE

namespace base
{
	class Connection
	{
	public:
		Connection() = default;

		Connection(const Connection&) = delete;

		Connection(Connection&&connection );

		Connection& operator=(const Connection&) = delete;

		Connection& operator=(Connection&& connection);

		Connection(std::function<void()>&& disconnect);

		~Connection();

		void disconnect();

	private:
		std::function<void()> m_disconnect;
	};
}

END_FTK_NAMESPACE
