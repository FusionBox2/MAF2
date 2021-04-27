/*=========================================================================

 Program: MAF2
 Module: mafString
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafString.h"
#include "mafMatrix.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "wx/wx.h"
#include <wx/string.h>
#include <string>

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

//----------------------------------------------------------------------------
int mafString::Replace(const mafString& s1, const mafString& s2, bool replaceAll)
//----------------------------------------------------------------------------
{
    wxString tmp = toWx();
    int res = tmp.Replace(s1.toWx(), s2.toWx(), replaceAll);
    *this = mafWxToString(tmp);
    return res;
}
#endif

//----------------------------------------------------------------------------
const mafID mafString::Length() const
//----------------------------------------------------------------------------
{
    return m_str.length();
}

//----------------------------------------------------------------------------
void mafString::NCopy(mafStrBuf str, int n)
//----------------------------------------------------------------------------
{
    m_str.append(str, n);
}

//----------------------------------------------------------------------------
void mafString::NCopy(const mafString& str, int n)
//----------------------------------------------------------------------------
{
    m_str.append(str.m_str, n);
}

//----------------------------------------------------------------------------
void mafString::Erase(int start, int end)
//----------------------------------------------------------------------------
{
    int len = Length();
    if (end == -1 || end >= len)
        end = len - 1;
    if (start > end || end < 0)
        return;
    m_str.erase(start, end - start + 1);
}

//----------------------------------------------------------------------------
int mafString::Compare(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return m_str.compare(str);
}

//----------------------------------------------------------------------------
int mafString::Compare(const mafString& str) const
//----------------------------------------------------------------------------
{
    return m_str.compare(str.m_str);
}

//----------------------------------------------------------------------------
bool mafString::Equals(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return Compare(str) == 0;
}

//----------------------------------------------------------------------------
bool mafString::Equals(const mafString& str) const
//----------------------------------------------------------------------------
{
    return Compare(str) == 0;
}

//----------------------------------------------------------------------------
bool mafString::StartsWith(mafStrBuf str) const
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
bool mafString::StartsWith(const mafString& str) const
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
bool mafString::EndsWith(mafStrBuf str) const
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
bool mafString::EndsWith(const mafString& str) const
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
mafString& mafString::Append(mafStrBuf str)
//----------------------------------------------------------------------------
{
    m_str.append(str);
    return *this;
}

//----------------------------------------------------------------------------
mafString& mafString::Append(const mafString& str)
//----------------------------------------------------------------------------
{
    m_str.append(str.m_str);
    return *this;
}

//----------------------------------------------------------------------------
int mafString::FindChr(const int c) const
//----------------------------------------------------------------------------
{
  auto pos = m_str.find_first_of(c);
  return (pos != std::basic_string<mafStringChar>::npos) ? pos : -1;
}

//----------------------------------------------------------------------------
int mafString::FindLastChr(const int c) const
//----------------------------------------------------------------------------
{
  auto pos = m_str.find_last_of(c);
  return (pos != std::basic_string<mafStringChar>::npos) ? pos : -1;
}

//----------------------------------------------------------------------------
int mafString::FindFirst(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    auto pos = m_str.find(str);
    return (pos != std::basic_string<mafStringChar>::npos) ? pos : -1;
}

//----------------------------------------------------------------------------
int mafString::FindFirst(const mafString& str) const
//----------------------------------------------------------------------------
{
  auto pos = m_str.find(str.m_str);
  return (pos != std::basic_string<mafStringChar>::npos) ? pos : -1;
}

//----------------------------------------------------------------------------
int mafString::FindLast(mafStrBuf str) const
//----------------------------------------------------------------------------
{
  auto pos = m_str.rfind(str);
  return (pos != std::basic_string<mafStringChar>::npos) ? pos : -1;
}

//----------------------------------------------------------------------------
int mafString::FindLast(const mafString& str) const
//----------------------------------------------------------------------------
{
  auto pos = m_str.find(str.m_str);
  return (pos != std::basic_string<mafStringChar>::npos) ? pos : -1;
}

//----------------------------------------------------------------------------
mafString mafString::BaseName() const
//----------------------------------------------------------------------------
{
  mafString res(*this);
  return res.MakeBaseName();
}

//----------------------------------------------------------------------------
mafString& mafString::MakeBaseName()
//----------------------------------------------------------------------------
{
#ifdef _WIN32
    auto pos = m_str.find_last_of("/\\");
#else
    auto pos = m_str.find_last_of("/");
#endif
    if (pos != std::basic_string<mafStringChar>::npos)
    {
        m_str.erase(0, pos + 1);
    }
    return *this;
}

//----------------------------------------------------------------------------
void mafString::ExtractPathName()
//----------------------------------------------------------------------------
{
    //wxString path, s;
    *this = mafWxToString(wxPathOnly(toWx()));
    //Set(path.c_str());

  /*  int idx=FindLastChr('/');

    if (idx>=0)
    {
      Erase(idx+1,-1);
    }
    else
    {
      Set("");
    }*/
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
mafString& mafString::Clear()
//----------------------------------------------------------------------------
{
    m_str.clear();
    return *this;
}

//----------------------------------------------------------------------------
void mafString::ParsePathName()
//----------------------------------------------------------------------------
{
    if (IsEmpty())
        return;
    // for Windows platforms parse the string to substitute "/" and "\\" with the right one.
#ifdef _WIN32
    mafID length = Length();
    unsigned start = 0;
    if (length >= 2)
    {
        if (m_str[0] == '\\' && m_str[1] == '\\')
            start = 2;
    }
    for (unsigned int i = start; i < length; i++)
    {
        if (m_str[i] == '\\')
            m_str[i] = '/';
    }
#endif
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

bool mafString::IsEmpty() const
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
char& mafString::operator [] (const int i)
//----------------------------------------------------------------------------
{
    return m_str[i];
}

//----------------------------------------------------------------------------
const char mafString::operator [] (const int i) const
//----------------------------------------------------------------------------
{
    return m_str[i];
}

//----------------------------------------------------------------------------
const bool mafString::operator==(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return Equals(str);
}

//----------------------------------------------------------------------------
const bool mafString::operator!=(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return Equals(str);
}

//----------------------------------------------------------------------------
const bool mafString::operator<(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return Compare(str) < 0;
}
//----------------------------------------------------------------------------
const bool mafString::operator>(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return Compare(str) > 0;
}
//----------------------------------------------------------------------------
const bool mafString::operator<=(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return Compare(str) <= 0;
}
//----------------------------------------------------------------------------
const bool mafString::operator>=(mafStrBuf str) const
//----------------------------------------------------------------------------
{
    return Compare(str) >= 0;
}

//----------------------------------------------------------------------------
const bool mafString::operator!=(const mafString& str) const
//----------------------------------------------------------------------------
{
  return !Equals(str);
}

//----------------------------------------------------------------------------
const bool mafString::operator==(const mafString& str) const
//----------------------------------------------------------------------------
{
  return Equals(str);
}

//----------------------------------------------------------------------------
const bool mafString::operator<(const mafString& str) const
//----------------------------------------------------------------------------
{
  return Compare(str) < 0;
}

//----------------------------------------------------------------------------
const bool mafString::operator>(const mafString& str) const
//----------------------------------------------------------------------------
{
  return Compare(str) > 0;
}

//----------------------------------------------------------------------------
const bool mafString::operator<=(const mafString& str) const
//----------------------------------------------------------------------------
{
  return Compare(str) <= 0;
}

//----------------------------------------------------------------------------
const bool mafString::operator>=(const mafString& str) const
//----------------------------------------------------------------------------
{
  return Compare(str) >= 0;
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
    return mafString::Format(_R("%d"), d);
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

//----------------------------------------------------------------------------
mafMessageBuf _M(const mafString& s)
//----------------------------------------------------------------------------
{
  return mafMessageBuf(s.GetCStr());
}
