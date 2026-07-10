#include "ftk/Base/String.h"


#ifdef MAF_USE_WX
#include "wx/wx.h"
#include <wx/string.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string>

BEGIN_FTK_NAMESPACE

namespace base
{
	String::String() = default;

	String::String(StrBuf str)
		: m_str(str)
	{
	}

	String& String::operator=(StrBuf str)
	{
		m_str = str;
		return *this;
	}

	String::size_type String::length() const
	{
		return m_str.length();
	}

	String::size_type String::size() const
	{
		return m_str.size();
	}

	void String::erase(String::size_type pos, String::size_type count)
	{
		m_str.erase(pos, count);
	}

	String String::substr(String::size_type pos, String::size_type count) const
	{
		String result;
		result.m_str = m_str.substr(pos, count);
		return result;
	}

	int String::compare(StrBuf str) const
	{
		return m_str.compare(str);
	}

	int String::compare(const String& str) const
	{
		return m_str.compare(str.m_str);
	}

	bool String::starts_with(StrBuf str) const
	{
#ifndef FUNCTIONAL_STRBUF
		size_t len = std::char_traits<StringChar>::length(str);
#else
		size_t len = std::char_traits<StringChar>::length(str.GetBuf());
#endif
		if (m_str.length() < len)
		{
			return false;
		}
		return m_str.compare(0, len, str) == 0;
	}

	bool String::starts_with(const String& str) const
	{
		size_t len = str.m_str.length();
		if (m_str.length() < len)
		{
			return false;
		}
		return m_str.compare(0, len, str.m_str) == 0;
	}

	bool String::ends_with(StrBuf str) const
	{
#ifndef FUNCTIONAL_STRBUF
		size_t len = std::char_traits<StringChar>::length(str);
#else
		size_t len = std::char_traits<StringChar>::length(str.GetBuf());
#endif
		if (m_str.length() < len)
		{
			return false;
		}
		return m_str.compare(m_str.length() - len, len, str) == 0;
	}

	bool String::ends_with(const String& str) const
	{
		size_t len = str.m_str.length();
		if (m_str.length() < len)
		{
			return false;
		}
		return m_str.compare(m_str.length() - len, len, str.m_str) == 0;
	}

	String& String::append(StrBuf str)
	{
		m_str.append(str);
		return *this;
	}

	String& String::append(const String& str)
	{
		m_str.append(str.m_str);
		return *this;
	}

	String::size_type String::find_first_of(StringChar c) const
	{
		return m_str.find_first_of(c);
	}

	String::size_type String::find_first_of(const String& str) const
	{
		return m_str.find_first_of(str.m_str);
	}

	String::size_type String::find_first_of(StrBuf str) const
	{
		return m_str.find_first_of(str);
	}

	String::size_type String::find_last_of(StringChar c) const
	{
		return m_str.find_last_of(c);
	}

	String::size_type String::find_last_of(const String& str) const
	{
		return m_str.find_last_of(str.m_str);
	}

	String::size_type String::find_last_of(StrBuf str) const
	{
		return m_str.find_last_of(str);
	}

	String::size_type String::find(StrBuf str) const
	{
		auto pos = m_str.find(str);
		return (pos != std::basic_string<StringChar>::npos) ? pos : npos;
	}

	String::size_type String::find(const String& str) const
	{
		auto pos = m_str.find(str.m_str);
		return (pos != std::basic_string<StringChar>::npos) ? pos : npos;
	}

	String::size_type String::rfind(StrBuf str) const
	{
		auto pos = m_str.rfind(str);
		return (pos != std::basic_string<StringChar>::npos) ? pos : npos;
	}

	String::size_type String::rfind(const String& str) const
	{
		auto pos = m_str.find(str.m_str);
		return (pos != std::basic_string<StringChar>::npos) ? pos : npos;
	}

	String ToUpper(const String& str)
	{
		String res(str);
		for (size_t i = 0; i < res.size(); i++)
		{
			res[i] = toupper(res[i]);
		}
		return res;
	}

	String ToLower(const String& str)
	{
		String res(str);
		for (size_t i = 0; i < res.size(); i++)
		{
			res[i] = tolower(res[i]);
		}
		return res;
	}

	void String::clear()
	{
		m_str.clear();
	}

	StrBuf String::c_str() const
	{
#ifndef FUNCTIONAL_STRBUF
		return m_str.c_str();
#else
		return StrBuf::MakeBuf(m_str.c_str());
#endif
	}

	StrBuf String::GetCStr() const
	{
		return c_str();
	}

	bool String::empty() const
	{
		return m_str.empty();
	}

	String String::Format(StrBuf format, ...)
	{
		const int BUF_SIZE = 2048;
		StringChar message[BUF_SIZE];
		StringChar* pText = message;
		unsigned char* pBuffer = nullptr;

		pText = message;
		va_list argList;
		va_start(argList, format);

		va_list argListBuf;
		va_copy(argListBuf, argList);
		int len = vsnprintf(pText, BUF_SIZE, format, argListBuf);
		va_end(argListBuf);

		if (len >= BUF_SIZE)
		{
			pBuffer = new (std::nothrow) unsigned char[(len + 1) * sizeof(StringChar)];
			if (pBuffer)
			{
				pText = reinterpret_cast<StringChar*>(pBuffer);
				len = vsnprintf(pText, len + 1, format, argList);
			}
		}
		va_end(argList);

		String res;
		res.m_str.assign(pText, len);
		if (pBuffer)
			delete[] pBuffer;
		return res;
	}

	StringChar& String::operator [] (String::size_type i)
	{
		return m_str[i];
	}

	StringChar String::operator [] (String::size_type i) const
	{
		return m_str[i];
	}

	bool String::operator==(StrBuf str) const
	{
		return compare(str) == 0;
	}

	bool String::operator!=(StrBuf str) const
	{
		return compare(str) != 0;
	}

	bool String::operator<(StrBuf str) const
	{
		return compare(str) < 0;
	}

	bool String::operator>(StrBuf str) const
	{
		return compare(str) > 0;
	}

	bool String::operator<=(StrBuf str) const
	{
		return compare(str) <= 0;
	}

	bool String::operator>=(StrBuf str) const
	{
		return compare(str) >= 0;
	}

	bool String::operator!=(const String& str) const
	{
		return compare(str) != 0;
	}

	bool String::operator==(const String& str) const
	{
		return compare(str) == 0;
	}

	bool String::operator<(const String& str) const
	{
		return compare(str) < 0;
	}

	bool String::operator>(const String& str) const
	{
		return compare(str) > 0;
	}

	bool String::operator<=(const String& str) const
	{
		return compare(str) <= 0;
	}

	bool String::operator>=(const String& str) const
	{
		return compare(str) >= 0;
	}

	String& String::operator+=(StrBuf str)
	{
		m_str += str;
		return *this;
	}

	String& String::operator+=(const String& str)
	{
		m_str += str.m_str;
		return *this;
	}

	String operator+(const String& s1, StrBuf s2)
	{
		String res(s1);
		return res += s2;
	}

	String operator+(StrBuf s1, const String& s2)
	{
		String res(s1);
		return res += s2;
	}

	String operator+(const String& s1, const String& s2)
	{
		String res(s1);
		return res += s2;
	}

	std::string StringToStd(const String& str)
	{
		return str.c_str().GetBuf();
	}

	String StdToString(const std::string& str)
	{
#ifndef FUNCTIONAL_STRBUF
		return str.c_str();
#else
		return StrBuf::MakeBuf(str.c_str());
#endif
	}
}

END_FTK_NAMESPACE
