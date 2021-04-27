/*=========================================================================

 Program: MAF2
 Module: mafString
 Authors: originally based on vtkString (www.vtk.org), rewritten Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafString_h
#define __mafString_h

#include "mafDefines.h"
#include "mafBase.h" 
#include <string.h>
#include <string>
//----------------------------------------------------------
// forward references:
//----------------------------------------------------------
class mafMatrix;

using mafStringChar = char;
#define FUNCTIONAL_STRBUF
#ifndef FUNCTIONAL_STRBUF
using mafStrBuf = const mafStringChar*;
#define __R(x) x
//#define __R(x) u8 ## x
#define _R(x) __R(x)
#define _L(x) __R(x)
#else
class MAF_EXPORT mafStrBuf
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
class MAF_EXPORT mafString : public mafBase
{
public:
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
    int Replace(const mafString& s1, const mafString& s2, bool replaceAll = true);
#endif
    
  /** This method returns the size of this string. */
  const mafID Length() const;

  /** Copy N characters of another string to this string.*/
  void NCopy(mafStrBuf src, int n);

  /** Copy N characters of another string to this string.*/
  void NCopy(const mafString& str, int n);

  /**  Erase characters from start position to end position. If end
    is not specified erase to the end of the string.*/
  void Erase(int start,int end=-1);

  /** 
    This method compare the given c-string with the one stored inside this object.
    It is similar to strcmp, but it can handle null pointers. Return 0 if str equal this,
    -1 if str > this, 1 if str < this*/
  int Compare(mafStrBuf str) const;

  /**
    This method compare the given c-string with the one stored inside this object.
    It is similar to strcmp, but it can handle null pointers. Return 0 if str equal this,
    -1 if str > this, 1 if str < this*/
  int Compare(const mafString& str) const;

  /**
    This method compare the given c-string with the one stored inside this object.
    It is similar to strcmp, but it can handle null pointers. Also it only
    returns C style true or false versus compare which returns also which
    one is greater.*/
  bool Equals(mafStrBuf str) const;

  /**
    This method compare the given c-string with the one stored inside this object.
    It is similar to strcmp, but it can handle null pointers. Also it only
    returns C style true or false versus compare which returns also which
    one is greater.*/
  bool Equals(const mafString& str) const;

  /** Check if this string starts with the given one.*/
  bool StartsWith(mafStrBuf str) const;

  /** Check if this string starts with the given one.*/
  bool StartsWith(const mafString& str) const;

  /** Check if this string ends with the given one.*/
  bool EndsWith(mafStrBuf str) const;

  /** Check if this string ends with the given one.*/
  bool EndsWith(const mafString& str) const;

  /** Append a new string to this string. */
  mafString &Append(mafStrBuf str);

  /** Append a new string to this string. */
  mafString& Append(const mafString& str);

  /** Scan the string for the first occurrence of the character */
  int FindChr(const int c) const;

  /** Scan the string for the first occurrence of the character */
  int FindLastChr(const int c) const;

  /** Find first occurrence of a substring */
  int FindFirst(const mafString& str) const;

  /** Find first occurrence of a substring */
  int FindFirst(mafStrBuf str) const;

  /** Find last occurrence of a substring */
  int FindLast(const mafString& str) const;

  /** Find last occurrence of a substring */
  int FindLast(mafStrBuf str) const;

  mafString BaseName() const;

  mafString& MakeBaseName();

  /** Extract the pathname from a filename string. Result is written inplace. */
  void ExtractPathName();

  mafString Upper()const;
  mafString Lower()const;

  mafString& MakeUpper();
  mafString& MakeLower();
  mafString& Clear();

  /**
    parse the given string to substitute each (back)slash
    character with the right pathname separator.*/
  void ParsePathName();

  /** Return the pointer to the internal c-string */
  mafStrBuf GetCStr() const;
  
  /**  return true if empty*/
  bool IsEmpty() const;

  /** Format given arguments according to format string. Format string format is
      that of vsprintf function */
  static mafString Format(mafStrBuf format, ...);

  /** 
    Direct access to single string elements for writing. This operator
    forces memory copy in case of internal const char reference. */
  mafStringChar& operator [] (const int i);

  /** direct access to string single elements for reading */
  const mafStringChar operator [] (const int i) const;

  const bool operator==(mafStrBuf str) const;
  const bool operator!=(mafStrBuf str) const;
  const bool operator<(mafStrBuf str) const;
  const bool operator>(mafStrBuf str) const;
  const bool operator<=(mafStrBuf str) const;
  const bool operator>=(mafStrBuf str) const;

  const bool operator!=(const mafString& str) const;
  const bool operator==(const mafString& str) const;
  const bool operator<(const mafString& str) const;
  const bool operator>(const mafString& str) const;
  const bool operator<=(const mafString& str) const;
  const bool operator>=(const mafString& str) const;

  mafString& operator+=(mafStrBuf str);
  mafString& operator+=(const mafString& str);

  std::string toStd() const;
private:
  std::basic_string<mafStringChar> m_str;
};

MAF_EXPORT mafString operator+(const mafString& s1, mafStrBuf s2);
MAF_EXPORT mafString operator+(mafStrBuf s1, const mafString& s2);
MAF_EXPORT mafString operator+(const mafString& s1, const mafString& s2);

MAF_EXPORT mafString mafWxToString(const wxString& str);

MAF_EXPORT mafString mafToString(int        d);
MAF_EXPORT mafString mafToString(long       d);
MAF_EXPORT mafString mafToString(float      d);
MAF_EXPORT mafString mafToString(double     d);
/** Put inside string a mafMatrix in row order
example:
1 0 0 0
  0 1 0 0
  0 0 1 0
  0 0 0 1
  -> "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1"
  */
MAF_EXPORT mafString mafToString(const mafMatrix& mat);

MAF_EXPORT mafMessageBuf _M(const char* s);
MAF_EXPORT mafMessageBuf _M(const mafString& s);
#endif

