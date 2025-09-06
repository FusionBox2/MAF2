#pragma once

#include "ftkConfigure.h"

#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

BEGIN_FTK_NAMESPACE

namespace utilities
{
	template <class T, std::size_t size, std::size_t align, bool strict = false>
	class FastPImpl final
	{
	public:
		FastPImpl(FastPImpl&& v) noexcept(noexcept(T(std::declval<T>()))) : FastPImpl(std::move(*v)) {}

		FastPImpl(const FastPImpl& v) noexcept(noexcept(T(std::declval<const T&>()))) : FastPImpl(*v) {}

		FastPImpl& operator=(const FastPImpl& rhs) noexcept(noexcept(std::declval<T&>() = std::declval<const T&>()))
		{
			if (&rhs != this)
			{
				*holder() = *rhs;
			}
			return *this;
		}

		FastPImpl& operator=(FastPImpl&& rhs) noexcept(noexcept(std::declval<T&>() = std::declval<T>()))
		{
			*holder() = std::move(*rhs);
			return *this;
		}

		template <typename... Args>
		explicit FastPImpl(Args&&... args) noexcept(noexcept(T(std::declval<Args>()...)))
		{
			::new (holder()) T(std::forward<Args>(args)...);
		}

		T* operator->() noexcept { return holder(); }

		const T* operator->() const noexcept { return holder(); }

		T& operator*() noexcept { return *holder(); }

		const T& operator*() const noexcept { return *holder(); }

		~FastPImpl() noexcept
		{
			Validate<sizeof(T), alignof(T), noexcept(std::declval<T*>()->~T())>();
			std::destroy_at(holder());
		}

	private:
		template <std::size_t actualSize, std::size_t actualAlign, bool actualNoexcept>
		static void Validate() noexcept
		{
			static_assert(!strict || size == actualSize, "invalid size: size == sizeof(T) failed");
			static_assert(size >= actualSize, "invalid size: size >= sizeof(T) failed");

			static_assert(!strict || align == actualAlign, "invalid align: align == alignof(T) failed");
			static_assert(align % actualAlign == 0, "invalid align: align % alignof(T) == 0 failed");

			static_assert(actualNoexcept, "Destructor of FastPImpl is marked as noexcept, the ~T() is not");
		}

		T* holder() noexcept { return reinterpret_cast<T*>(&storage_); }

		const T* holder() const noexcept { return reinterpret_cast<const T*>(&storage_); }

		alignas(align) std::byte storage_[size];
	};
}

END_FTK_NAMESPACE
