#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Connection.h"

#include <functional>
#include <memory>
#include <unordered_map>

BEGIN_FTK_NAMESPACE

namespace base
{
	template <typename... Args>
	class Signal
	{
	public:
		Signal()
		{
			m_slots = std::make_shared<std::unordered_map<size_t, std::function<void(Args...)>>>();
		}

		Connection connect(std::function<void(Args...)> slot)
		{
			auto id = m_nextId++;
			m_slots->emplace(id, std::move(slot));
			return Connection(
				[p = std::weak_ptr<std::unordered_map<size_t, std::function<void(Args...)>>>(m_slots), id]()
				{
					if (auto slots = p.lock())
					{
						slots->erase(id);
					}
				});
		}

		void emit(Args... args) const
		{
			for (auto& [id, slot] : *m_slots)
			{
				slot(args...);
			}
		}

	private:
		std::shared_ptr<std::unordered_map<size_t, std::function<void(Args...)>>> m_slots;
		size_t m_nextId = 0;
	};
}
END_FTK_NAMESPACE
