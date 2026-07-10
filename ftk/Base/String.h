#pragma once

#include "ftkConfigure.h"

#include <string>

#define FUNCTIONAL_STRBUF

#ifdef MAF_USE_WX
class wxString;
#endif

BEGIN_FTK_NAMESPACE

namespace base
{
	using StringChar = char;
#ifndef FUNCTIONAL_STRBUF
	using StrBuf = const StringChar*;
#else
	class FTK_BASE_EXPORT StrBuf
	{
	public:
		const StringChar* GetBuf() const { return m_buf; }
		operator const StringChar* () const { return m_buf; }
		StrBuf& operator=(const StrBuf& other)
		{
			m_buf = other.m_buf;
			return *this;
		}
		static StrBuf MakeBuf(const StringChar* buf)
		{
			return StrBuf(buf);
		}
	private:
		StrBuf(const StringChar* buf)
			: m_buf(buf ? buf : "")
		{
		}
		StrBuf(const wxString& str);
		StrBuf& operator=(const wxString& str);
		StrBuf& operator=(const StringChar* buf);
		const StringChar* m_buf;
	};
#endif

	class String
	{
	public:
		using size_type = std::size_t;
		static constexpr auto npos{ static_cast<size_type>(-1) };

		String();

		String(StrBuf str);

		String& operator=(StrBuf str);

		size_type length() const;

		size_type size() const;

		void erase(size_type pos = 0, size_type count = npos);

		String substr(size_type pos = 0, size_type count = npos) const;

		int compare(StrBuf str) const;

		int compare(const String& str) const;

		bool starts_with(StrBuf str) const;

		bool starts_with(const String& str) const;

		bool ends_with(StrBuf str) const;

		bool ends_with(const String& str) const;

		String& append(StrBuf str);

		String& append(const String& str);

		size_type find_first_of(StringChar c) const;

		size_type find_first_of(const String& str) const;

		size_type find_first_of(StrBuf str) const;

		size_type find_last_of(StringChar c) const;

		size_type find_last_of(const String& str) const;

		size_type find_last_of(StrBuf str) const;

		size_type find(const String& str) const;

		size_type find(StrBuf str) const;

		size_type rfind(const String& str) const;

		size_type rfind(StrBuf str) const;

		void clear();

		StrBuf GetCStr() const;

		StrBuf c_str() const;

		bool empty() const;

		static String Format(StrBuf format, ...);

		StringChar& operator [] (size_type i);

		StringChar operator [] (size_type i) const;

		bool operator==(StrBuf str) const;
		bool operator!=(StrBuf str) const;
		bool operator<(StrBuf str) const;
		bool operator>(StrBuf str) const;
		bool operator<=(StrBuf str) const;
		bool operator>=(StrBuf str) const;

		bool operator!=(const String& str) const;
		bool operator==(const String& str) const;
		bool operator<(const String& str) const;
		bool operator>(const String& str) const;
		bool operator<=(const String& str) const;
		bool operator>=(const String& str) const;

		String& operator+=(StrBuf str);

		String& operator+=(const String& str);

	private:
		std::basic_string<StringChar> m_str;
	};

	FTK_BASE_EXPORT String operator+(const String& s1, StrBuf s2);
	FTK_BASE_EXPORT String operator+(StrBuf s1, const String& s2);
	FTK_BASE_EXPORT String operator+(const String& s1, const String& s2);

	std::string StringToStd(const String& str);
	String StdToString(const std::string& str);

	String ToUpper(const String& str);
	String ToLower(const String& str);
}

namespace std
{
	template<>
	struct hash<base::String>
	{
		size_t operator()(const base::String& s) const noexcept
		{
			return std::hash<decltype(base::StringToStd(s))>{}(base::StringToStd(s));
		}
	};
}

#ifndef FUNCTIONAL_STRBUF
#define __R(x) x
//#define __R(x) u8 ## x
#define _R(x) __R(x)
#define _L(x) __R(x)
#else
#define _R(x) base::StrBuf::MakeBuf(x)
#define _L(x) base::StrBuf::MakeBuf(x)
#endif

END_FTK_NAMESPACE
