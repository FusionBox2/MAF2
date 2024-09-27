#pragma once

#include "ftkConfigure.h"

#include <utility>

BEGIN_FTK_NAMESPACE

template <class T>
class RegisteringPointer
{
public:  
	RegisteringPointer(T* r = nullptr, void *owner = nullptr) noexcept
		: m_Object(r)
	{
		if (m_Object)
		{
			m_Object->Register(owner);
		}
	}

	RegisteringPointer(const RegisteringPointer& r) noexcept
		: RegisteringPointer(r.m_Object)
	{
	}
  
	template <class U>
	RegisteringPointer(const RegisteringPointer<U>& r) noexcept
		: RegisteringPointer(r.m_Object)
	{
	}

	RegisteringPointer(RegisteringPointer&& r) noexcept
		: m_Object(r.m_Object)
	{
		r.m_Object = nullptr;
	}

	template <class U>
	RegisteringPointer(RegisteringPointer<U>&& r) noexcept
		: m_Object(r.m_Object)
	{
		r.m_Object = nullptr;
	}

	~RegisteringPointer()
	{
		if (m_Object)
		{
			m_Object->UnRegister(nullptr);
			m_Object = nullptr;
		}
	}
  
	RegisteringPointer& operator=(T* r)
	{
		RegisteringPointer tmp(r);
		std::swap(m_Object, tmp.m_Object);
		return *this;
	}

	RegisteringPointer& operator=(const RegisteringPointer<T>& r)
	{
		// make use of a temp auto-ptr to safely unregister, this
		// to avoid deallocation in case of self assignment
		RegisteringPointer<T> tmp(r);
		std::swap(m_Object, tmp.m_Object);
		return *this;
	}

	template <class U>
	RegisteringPointer& operator=(const RegisteringPointer<U>& r)
	{
		// make use of a temp auto-ptr to safely unregister, this
		// to avoid deallocation in case of self assignment
		RegisteringPointer<T> tmp(r);
		std::swap(m_Object, tmp.m_Object);
		return *this;
	}

	RegisteringPointer& operator=(RegisteringPointer<T>&& r)
	{
		// make use of a temp auto-ptr to safely unregister, this
		// to avoid deallocation in case of self assignment
		RegisteringPointer<T> tmp(std::move(r));
		std::swap(m_Object, tmp.m_Object);
		return *this;
	}

	template <class U>
	RegisteringPointer& operator=(RegisteringPointer<U>&& r)
	{
		// make use of a temp auto-ptr to safely unregister, this
		// to avoid deallocation in case of self assignment
		RegisteringPointer<T> tmp(std::move(r));
		std::swap(m_Object, tmp.m_Object);
		return *this;
	}

	T* get() const { return m_Object; }
  
  T* operator->() const { return get(); }

	T& operator*() const { return *get(); }

	//operator bool() const { return get(); }
  
protected:
  T* m_Object;
};

template <class U1, class U2>
bool operator==(const RegisteringPointer<U1>& x, const RegisteringPointer<U2>& y) { return x.get() == y.get(); }
template <class U>
bool operator==(const RegisteringPointer<U>& x, std::nullptr_t) { return x.get() == nullptr; }
template <class U>
bool operator==(std::nullptr_t, const RegisteringPointer<U>& x) { return x.get() == nullptr; }

template <class U1, class U2>
bool operator!=(const RegisteringPointer<U1>& x, const RegisteringPointer<U2>& y) { return x.get() != y.get(); }
template <class U>
bool operator!=(const RegisteringPointer<U>& x, std::nullptr_t) { return x.get() != nullptr; }
template <class U>
bool operator!=(std::nullptr_t, const RegisteringPointer<U>& x) { return x.get() != nullptr; }

template <class T>
using mafAutoPointer = RegisteringPointer<T>;

END_FTK_NAMESPACE
