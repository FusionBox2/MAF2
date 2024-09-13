#pragma once

#include "ftkConfigure.h"

#include "mafDefines.h"
#include <string.h>
#include <string>
//----------------------------------------------------------
// forward references:
//----------------------------------------------------------
class mafMatrix;

BEGIN_FTK_NAMESPACE

using mafStringChar = char;
#define FUNCTIONAL_STRBUF
#ifndef FUNCTIONAL_STRBUF
using mafStrBuf = const mafStringChar*;
#define __R(x) x
//#define __R(x) u8 ## x
#define _R(x) __R(x)
#define _L(x) __R(x)
#else
class FTK_BASE_EXPORT mafStrBuf
{
public:
    const mafStringChar* GetBuf() const { return m_buf; }
    operator const mafStringChar* () const { return m_buf; }
    mafStrBuf& operator=(const mafStrBuf& other)
    {
        m_buf = other.m_buf;
        return *this;
    }
    static mafStrBuf MakeBuf(const mafStringChar* buf)
    {
        return mafStrBuf(buf);
    }
private:
    mafStrBuf(const mafStringChar* buf)
        : m_buf(buf ? buf : "")
    {}
    mafStrBuf(const wxString& str);
    mafStrBuf& operator=(const wxString& str);
    mafStrBuf& operator=(const mafStringChar* buf);
    const mafStringChar* m_buf;
};
#define _R(x) mafStrBuf::MakeBuf(x)
#define _L(x) mafStrBuf::MakeBuf(x)
#endif

/** mafString - performs common string operations on c-strings.
  mafString is an implementation of string which operates on a traditional
  c-string internally stored. At each moment this string can be retrieved with
  GetCStr(). The memory of this m_CStr is automatically garbaged. Originally based
  on vtkString. BEWARE, when mafString is assigned with Set() to a "const char *" this
  is simply referenced and not copied, this implies very high performance but can cause
  invalid  memory access: e.g. in case the "const char *" is a function argument.
  When a mafString storing a reference to a const char * is modified the string is 
  automatically copied into a new memory.
  This string can be passed as argument wherever a const char * is expected.
  @sa mafCString
  */
class FTK_BASE_EXPORT mafString
{
public:
    using size_type = std::size_t;
	static constexpr auto npos{ static_cast<size_type>(-1) };

    mafString();

    mafString(mafStrBuf str);
    mafString& operator=(mafStrBuf str);

#ifdef MAF_USE_WX
private:
    mafString(const wxString& str);
    mafString& operator=(const wxString& str);
    mafString& operator+=(const wxString& str);
public:
    wxString toWx() const;
#endif
    
  /** This method returns the size of this string. */
  size_type length() const;

  size_type size() const;

  /**  Erase characters from start position to end position. If end
    is not specified erase to the end of the string.*/
  void erase(size_type pos = 0, size_type count = npos);

  mafString substr(size_type pos = 0, size_type count = npos);

  /**
    This method compare the given c-string with the one stored inside this object.
    It is similar to strcmp, but it can handle null pointers. Return 0 if str equal this,
    -1 if str > this, 1 if str < this*/
  int compare(mafStrBuf str) const;

  /**
    This method compare the given c-string with the one stored inside this object.
    It is similar to strcmp, but it can handle null pointers. Return 0 if str equal this,
    -1 if str > this, 1 if str < this*/
  int compare(const mafString& str) const;

  /** Check if this string starts with the given one.*/
  bool starts_with(mafStrBuf str) const;

  /** Check if this string starts with the given one.*/
  bool starts_with(const mafString& str) const;

  /** Check if this string ends with the given one.*/
  bool ends_with(mafStrBuf str) const;

  /** Check if this string ends with the given one.*/
  bool ends_with(const mafString& str) const;

  /** Append a new string to this string. */
  mafString &append(mafStrBuf str);

  /** Append a new string to this string. */
  mafString& append(const mafString& str);

  /** Scan the string for the first occurrence of the character */
  size_type find_first_of(mafStringChar c) const;

  /** Scan the string for the first occurrence of the character */
  size_type find_first_of(const mafString& str) const;

  /** Scan the string for the first occurrence of the character */
  size_type find_first_of(mafStrBuf str) const;

  /** Scan the string for the first occurrence of the character */
  size_type find_last_of(mafStringChar c) const;

  /** Scan the string for the first occurrence of the character */
  size_type find_last_of(const mafString& str) const;

  /** Scan the string for the first occurrence of the character */
  size_type find_last_of(mafStrBuf str) const;

  /** Find first occurrence of a substring */
  size_type find(const mafString& str) const;

  /** Find first occurrence of a substring */
  size_type find(mafStrBuf str) const;

  /** Find last occurrence of a substring */
  size_type rfind(const mafString& str) const;

  /** Find last occurrence of a substring */
  size_type rfind(mafStrBuf str) const;

  mafString Upper()const;
  mafString Lower()const;

  mafString& MakeUpper();
  mafString& MakeLower();
  void clear();

  /** Return the pointer to the internal c-string */
  mafStrBuf GetCStr() const;
  
  /**  return true if empty*/
  bool empty() const;

  /** Format given arguments according to format string. Format string format is
      that of vsprintf function */
  static mafString Format(mafStrBuf format, ...);

  /** 
    Direct access to single string elements for writing. This operator
    forces memory copy in case of internal const char reference. */
  mafStringChar& operator [] (size_type i);

  /** direct access to string single elements for reading */
  mafStringChar operator [] (size_type i) const;

  bool operator==(mafStrBuf str) const;
  bool operator!=(mafStrBuf str) const;
  bool operator<(mafStrBuf str) const;
  bool operator>(mafStrBuf str) const;
  bool operator<=(mafStrBuf str) const;
  bool operator>=(mafStrBuf str) const;

  bool operator!=(const mafString& str) const;
  bool operator==(const mafString& str) const;
  bool operator<(const mafString& str) const;
  bool operator>(const mafString& str) const;
  bool operator<=(const mafString& str) const;
  bool operator>=(const mafString& str) const;

  mafString& operator+=(mafStrBuf str);
  mafString& operator+=(const mafString& str);

  std::string toStd() const;
private:
  std::basic_string<mafStringChar> m_str;
};

FTK_BASE_EXPORT mafString operator+(const mafString& s1, mafStrBuf s2);
FTK_BASE_EXPORT mafString operator+(mafStrBuf s1, const mafString& s2);
FTK_BASE_EXPORT mafString operator+(const mafString& s1, const mafString& s2);

FTK_BASE_EXPORT mafString mafWxToString(const wxString& str);

FTK_BASE_EXPORT mafString mafToString(int        d);
FTK_BASE_EXPORT mafString mafToString(long       d);
FTK_BASE_EXPORT mafString mafToString(long long  d);
FTK_BASE_EXPORT mafString mafToString(float      d);
FTK_BASE_EXPORT mafString mafToString(double     d);
/** Put inside string a mafMatrix in row order
example:
1 0 0 0
  0 1 0 0
  0 0 1 0
  0 0 0 1
  -> "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1"
  */
FTK_BASE_EXPORT mafString mafToString(const mafMatrix& mat);
FTK_BASE_EXPORT mafString mafToString(double* comps, int num);
FTK_BASE_EXPORT mafString mafToString(int* comps, int num);
FTK_BASE_EXPORT mafString mafToString(const std::vector<double>& comps);
FTK_BASE_EXPORT mafString mafToString(const std::vector<int>& comps);

void mafParseVector(const mafString& str, double* comps, unsigned int num);
void mafParseVector(const mafString& str, int* comps, unsigned int num);
void mafParseVector(const mafString& str, std::vector<double>& comps);
void mafParseVector(const mafString& str, std::vector<int>& comps);

FTK_BASE_EXPORT mafMessageBuf _M(const char* s);
FTK_BASE_EXPORT mafMessageBuf _M(const mafString& s);

END_FTK_NAMESPACE
