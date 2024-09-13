#include "ftk/Base/String.h"
#include "mafMatrix.h"


#ifdef MAF_USE_WX
#include "wx/wx.h"
#include <wx/string.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string>

BEGIN_FTK_NAMESPACE

//----------------------------------------------------------------------------
mafString::mafString()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
mafString::mafString(mafStrBuf str):m_str(str)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
mafString &mafString::operator=(mafStrBuf str)
//----------------------------------------------------------------------------
{
  m_str = str;
  return *this;
}

#ifdef MAF_USE_WX
//----------------------------------------------------------------------------
wxString mafString::toWx() const
//----------------------------------------------------------------------------
{
    return m_str.c_str();
}
#endif

//----------------------------------------------------------------------------
mafString::size_type mafString::length() const
//----------------------------------------------------------------------------
{
    return m_str.length();
}

//----------------------------------------------------------------------------
mafString::size_type mafString::size() const
//----------------------------------------------------------------------------
{
	return m_str.size();
}

//----------------------------------------------------------------------------
void mafString::erase(mafString::size_type pos, mafString::size_type count)
//----------------------------------------------------------------------------
{
    m_str.erase(pos, count);
}

mafString mafString::substr(mafString::size_type pos, mafString::size_type count)
{
    mafString result;
    result.m_str = m_str.substr(pos, count);
    return result;
}

//----------------------------------------------------------------------------
int mafString::compare(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return m_str.compare(str);
}

//----------------------------------------------------------------------------
int mafString::compare(const mafString& str) const
//----------------------------------------------------------------------------
{
    return m_str.compare(str.m_str);
}

//----------------------------------------------------------------------------
bool mafString::starts_with(mafStrBuf str) const
//----------------------------------------------------------------------------
{
#ifndef FUNCTIONAL_STRBUF
    size_t len = std::char_traits<mafStringChar>::length(str);
#else
    size_t len = std::char_traits<mafStringChar>::length(str.GetBuf());
#endif
    if (m_str.length() < len)
    {
        return false;
    }
    return m_str.compare(0, len, str) == 0;
}

//----------------------------------------------------------------------------
bool mafString::starts_with(const mafString& str) const
//----------------------------------------------------------------------------
{
    size_t len = str.m_str.length();
    if (m_str.length() < len)
    {
        return false;
    }
    return m_str.compare(0, len, str.m_str) == 0;
}

//----------------------------------------------------------------------------
bool mafString::ends_with(mafStrBuf str) const
//----------------------------------------------------------------------------
{
#ifndef FUNCTIONAL_STRBUF
    size_t len = std::char_traits<mafStringChar>::length(str);
#else
    size_t len = std::char_traits<mafStringChar>::length(str.GetBuf());
#endif
    if (m_str.length() < len)
    {
        return false;
    }
    return m_str.compare(m_str.length() - len, len, str) == 0;
}

//----------------------------------------------------------------------------
bool mafString::ends_with(const mafString& str) const
//----------------------------------------------------------------------------
{
    size_t len = str.m_str.length();
    if (m_str.length() < len)
    {
        return false;
    }
    return m_str.compare(m_str.length() - len, len, str.m_str) == 0;
}

//----------------------------------------------------------------------------
mafString& mafString::append(mafStrBuf str)
//----------------------------------------------------------------------------
{
    m_str.append(str);
    return *this;
}

//----------------------------------------------------------------------------
mafString& mafString::append(const mafString& str)
//----------------------------------------------------------------------------
{
    m_str.append(str.m_str);
    return *this;
}

//----------------------------------------------------------------------------
mafString::size_type mafString::find_first_of(mafStringChar c) const
//----------------------------------------------------------------------------
{
  return m_str.find_first_of(c);
}

//----------------------------------------------------------------------------
mafString::size_type mafString::find_first_of(const mafString& str) const
//----------------------------------------------------------------------------
{
	return m_str.find_first_of(str.m_str);
}

//----------------------------------------------------------------------------
mafString::size_type mafString::find_first_of(mafStrBuf str) const
//----------------------------------------------------------------------------
{
	return m_str.find_first_of(str);
}

//----------------------------------------------------------------------------
mafString::size_type mafString::find_last_of(mafStringChar c) const
//----------------------------------------------------------------------------
{
  return m_str.find_last_of(c);
}

//----------------------------------------------------------------------------
mafString::size_type mafString::find_last_of(const mafString& str) const
//----------------------------------------------------------------------------
{
	return m_str.find_last_of(str.m_str);
}

//----------------------------------------------------------------------------
mafString::size_type mafString::find_last_of(mafStrBuf str) const
//----------------------------------------------------------------------------
{
	return m_str.find_last_of(str);
}

//----------------------------------------------------------------------------
mafString::size_type mafString::find(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    auto pos = m_str.find(str);
    return (pos != std::basic_string<mafStringChar>::npos) ? pos : -1;
}

//----------------------------------------------------------------------------
mafString::size_type mafString::find(const mafString& str) const
//----------------------------------------------------------------------------
{
  auto pos = m_str.find(str.m_str);
  return (pos != std::basic_string<mafStringChar>::npos) ? pos : -1;
}

//----------------------------------------------------------------------------
mafString::size_type mafString::rfind(mafStrBuf str) const
//----------------------------------------------------------------------------
{
  auto pos = m_str.rfind(str);
  return (pos != std::basic_string<mafStringChar>::npos) ? pos : -1;
}

//----------------------------------------------------------------------------
mafString::size_type mafString::rfind(const mafString& str) const
//----------------------------------------------------------------------------
{
  auto pos = m_str.find(str.m_str);
  return (pos != std::basic_string<mafStringChar>::npos) ? pos : -1;
}


//----------------------------------------------------------------------------
mafString mafString::Upper()const
//----------------------------------------------------------------------------
{
  mafString res(*this);
  return res.MakeUpper();
}

//----------------------------------------------------------------------------
mafString mafString::Lower()const
//----------------------------------------------------------------------------
{
  mafString res(*this);
  return res.MakeLower();
}

//----------------------------------------------------------------------------
mafString& mafString::MakeUpper()
//----------------------------------------------------------------------------
{
  for (auto& c : m_str)
  {
    c = toupper(c);
  }
  return *this;
}

//----------------------------------------------------------------------------
mafString& mafString::MakeLower()
//----------------------------------------------------------------------------
{
  for (auto& c : m_str)
  {
    c = tolower(c);
  }
  return *this;
}

//----------------------------------------------------------------------------
void mafString::clear()
//----------------------------------------------------------------------------
{
    m_str.clear();
}

//----------------------------------------------------------------------------
mafStrBuf mafString::GetCStr() const
//----------------------------------------------------------------------------
{
#ifndef FUNCTIONAL_STRBUF
    return m_str.c_str();
#else
    return mafStrBuf::MakeBuf(m_str.c_str());
#endif
}

bool mafString::empty() const
{
    return m_str.empty();
}

//----------------------------------------------------------------------------
mafString mafString::Format(mafStrBuf format, ...)
//----------------------------------------------------------------------------
{
    const int BUF_SIZE = 2048;
    mafStringChar message[BUF_SIZE];
    mafStringChar* pText = message;
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
        pBuffer = new (std::nothrow) unsigned char[(len + 1) * sizeof(mafStringChar)];
        if (pBuffer)
        {
            pText = reinterpret_cast<mafStringChar*>(pBuffer);
            len = vsnprintf(pText, len + 1, format, argList);
        }
    }
    va_end(argList);

    mafString res;
    res.m_str.assign(pText, len);
    if (pBuffer)
        delete[] pBuffer;
    return res;
}

//----------------------------------------------------------------------------
mafStringChar& mafString::operator [] (mafString::size_type i)
//----------------------------------------------------------------------------
{
    return m_str[i];
}

//----------------------------------------------------------------------------
mafStringChar mafString::operator [] (mafString::size_type i) const
//----------------------------------------------------------------------------
{
    return m_str[i];
}

//----------------------------------------------------------------------------
bool mafString::operator==(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return compare(str) == 0;
}

//----------------------------------------------------------------------------
bool mafString::operator!=(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return compare(str) != 0;
}

//----------------------------------------------------------------------------
bool mafString::operator<(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return compare(str) < 0;
}
//----------------------------------------------------------------------------
bool mafString::operator>(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return compare(str) > 0;
}
//----------------------------------------------------------------------------
bool mafString::operator<=(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return compare(str) <= 0;
}
//----------------------------------------------------------------------------
bool mafString::operator>=(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return compare(str) >= 0;
}

//----------------------------------------------------------------------------
bool mafString::operator!=(const mafString& str) const
//----------------------------------------------------------------------------
{
  return compare(str) != 0;
}

//----------------------------------------------------------------------------
bool mafString::operator==(const mafString& str) const
//----------------------------------------------------------------------------
{
  return compare(str) == 0;
}

//----------------------------------------------------------------------------
bool mafString::operator<(const mafString& str) const
//----------------------------------------------------------------------------
{
  return compare(str) < 0;
}

//----------------------------------------------------------------------------
bool mafString::operator>(const mafString& str) const
//----------------------------------------------------------------------------
{
  return compare(str) > 0;
}

//----------------------------------------------------------------------------
bool mafString::operator<=(const mafString& str) const
//----------------------------------------------------------------------------
{
  return compare(str) <= 0;
}

//----------------------------------------------------------------------------
bool mafString::operator>=(const mafString& str) const
//----------------------------------------------------------------------------
{
  return compare(str) >= 0;
}

//----------------------------------------------------------------------------
mafString& mafString::operator+=(mafStrBuf str)
//----------------------------------------------------------------------------
{
    m_str += str;
    return *this;
}

//----------------------------------------------------------------------------
mafString& mafString::operator+=(const mafString& str)
//----------------------------------------------------------------------------
{
    m_str += str.m_str;
    return *this;
}

//----------------------------------------------------------------------------
std::string mafString::toStd() const
//----------------------------------------------------------------------------
{
    return m_str;
}

//----------------------------------------------------------------------------
mafString operator+(const mafString& s1, mafStrBuf s2)
//----------------------------------------------------------------------------
{
    mafString res(s1);
    return res += s2;
}

//----------------------------------------------------------------------------
mafString operator+(mafStrBuf s1, const mafString& s2)
//----------------------------------------------------------------------------
{
    mafString res(s1);
    return res += s2;
}

//----------------------------------------------------------------------------
mafString operator+(const mafString& s1, const mafString& s2)
//----------------------------------------------------------------------------
{
    mafString res(s1);
    return res += s2;
}

//----------------------------------------------------------------------------
mafString mafWxToString(const wxString& str)
//----------------------------------------------------------------------------
{
#ifndef FUNCTIONAL_STRBUF
    return str.c_str();
#else
    return mafStrBuf::MakeBuf(str.c_str());
#endif
}

//----------------------------------------------------------------------------
mafString mafToString(int d)
//----------------------------------------------------------------------------
{
    return mafString::Format(_R("%d"), d);
}

//----------------------------------------------------------------------------
mafString mafToString(long d)
//----------------------------------------------------------------------------
{
    return mafString::Format(_R("%ld"), d);
}

//----------------------------------------------------------------------------
mafString mafToString(long long d)
//----------------------------------------------------------------------------
{
    return mafString::Format(_R("%lld"), d);
}

//----------------------------------------------------------------------------
mafString mafToString(float d)
//----------------------------------------------------------------------------
{
    return mafString::Format(_R("%.7g"), d);
}

//----------------------------------------------------------------------------
mafString mafToString(double d)
//----------------------------------------------------------------------------
{
    return mafString::Format(_R("%.16g"), d);
}

//----------------------------------------------------------------------------
mafString mafToString(const mafMatrix& mat)
//----------------------------------------------------------------------------
{
    mafString res;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            res += mafString::Format(_R("%.16g "), mat.GetElement(i, j));
        }
    }
    return res;
}

//------------------------------------------------------------------------------
template <class T>
mafString InternalStoreVectorN(T* comps, size_t num)
//------------------------------------------------------------------------------
{
	// Write all the elements into as a single 3-tupla  
	mafString elements;
	for (size_t i = 0; i < num; i++)
	{
		elements += mafToString(comps[i]) + _R(" ");
	}
	return elements;
}
//------------------------------------------------------------------------------
mafString mafToString(double* comps, int num)
//------------------------------------------------------------------------------
{
	assert(comps);
	return InternalStoreVectorN(comps, num);
}

//------------------------------------------------------------------------------
mafString mafToString(int* comps, int num)
//------------------------------------------------------------------------------
{
	assert(comps);
	return InternalStoreVectorN(comps, num);
}
//------------------------------------------------------------------------------
mafString mafToString(const std::vector<double>& comps)
//------------------------------------------------------------------------------
{
	return InternalStoreVectorN(comps.data(), comps.size());
}

//------------------------------------------------------------------------------
mafString mafToString(const std::vector<int>& comps)
//------------------------------------------------------------------------------
{
	return InternalStoreVectorN(comps.data(), comps.size());
}

//----------------------------------------------------------------------------
mafMessageBuf _M(const mafString& s)
//----------------------------------------------------------------------------
{
  return mafMessageBuf(s.GetCStr());
}

//------------------------------------------------------------------------------
template <class T>
size_t InternalParseData(const mafString& text, T* vector, size_t size)
//------------------------------------------------------------------------------
{
#pragma message ("potentially hacky")
	std::istringstream instr(text.toStd());

	for (size_t i = 0; i < size; i++)
	{
		if (instr.eof())
			return i;

		instr >> vector[i];
	}

	return size;
}
//------------------------------------------------------------------------------
void mafParseVector(const mafString& str, double* comps, unsigned int num)
//------------------------------------------------------------------------------
{
	size_t parsedElems = 0;
	if (!str.empty())
		parsedElems = InternalParseData(str, comps, num);
	if (parsedElems == num)
		return;
	//mafWarningMacro("Storage Parse Error while parsing <" << GetName().GetCStr() << "> element: wrong number of fields inside Storage element.");
}

//------------------------------------------------------------------------------
void mafParseVector(const mafString& str, int* comps, unsigned int num)
//------------------------------------------------------------------------------
{
	size_t parsedElems = 0;
	if (!str.empty())
		parsedElems = InternalParseData(str, comps, num);
	if (parsedElems == num)
		return;
	//mafWarningMacro("Storage Parse Error while parsing <"<<GetName().GetCStr()<<"> element: wrong number of fields inside Storage element." );
}
//------------------------------------------------------------------------------
void mafParseVector(const mafString& str, std::vector<double>& comps)
//------------------------------------------------------------------------------
{
	mafParseVector(str, comps.data(), comps.size());
}
//------------------------------------------------------------------------------
void mafParseVector(const mafString& str, std::vector<int>& comps)
//------------------------------------------------------------------------------
{
	mafParseVector(str, comps.data(), comps.size());
}

END_FTK_NAMESPACE
