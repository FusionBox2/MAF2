#pragma once

#include "ftkConfigure.h"

#include "mafDefines.h"

#include <string>

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
	{
	}
	mafStrBuf(const wxString& str);
	mafStrBuf& operator=(const wxString& str);
	mafStrBuf& operator=(const mafStringChar* buf);
	const mafStringChar* m_buf;
};
#define _R(x) mafStrBuf::MakeBuf(x)
#define _L(x) mafStrBuf::MakeBuf(x)
#endif

class FTK_BASE_EXPORT mafString
{
public:
	using size_type = std::size_t;
	static constexpr auto npos{ static_cast<size_type>(-1) };

	mafString();

	mafString(mafStrBuf str);
	mafString& operator=(mafStrBuf str);

	size_type length() const;

	size_type size() const;

	void erase(size_type pos = 0, size_type count = npos);

	mafString substr(size_type pos = 0, size_type count = npos) const;

	int compare(mafStrBuf str) const;

	int compare(const mafString& str) const;

	bool starts_with(mafStrBuf str) const;

	bool starts_with(const mafString& str) const;

	bool ends_with(mafStrBuf str) const;

	bool ends_with(const mafString& str) const;

	mafString& append(mafStrBuf str);

	mafString& append(const mafString& str);

	size_type find_first_of(mafStringChar c) const;

	size_type find_first_of(const mafString& str) const;

	size_type find_first_of(mafStrBuf str) const;

	size_type find_last_of(mafStringChar c) const;

	size_type find_last_of(const mafString& str) const;

	size_type find_last_of(mafStrBuf str) const;

	size_type find(const mafString& str) const;

	size_type find(mafStrBuf str) const;

	size_type rfind(const mafString& str) const;

	size_type rfind(mafStrBuf str) const;

	void clear();

	mafStrBuf GetCStr() const;

	mafStrBuf c_str() const;

	bool empty() const;

	static mafString Format(mafStrBuf format, ...);

	mafStringChar& operator [] (size_type i);

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

private:
	std::basic_string<mafStringChar> m_str;
};

FTK_BASE_EXPORT mafString operator+(const mafString& s1, mafStrBuf s2);
FTK_BASE_EXPORT mafString operator+(mafStrBuf s1, const mafString& s2);
FTK_BASE_EXPORT mafString operator+(const mafString& s1, const mafString& s2);

#ifdef MAF_USE_WX
FTK_BASE_EXPORT mafString mafWxToString(const wxString& str);
FTK_BASE_EXPORT wxString mafStringToWx(const mafString& str);
#endif
std::string mafStringToStd(const mafString& str);
mafString mafStdToString(const std::string& str);

mafString ToUpper(const mafString& str);
mafString ToLower(const mafString& str);

FTK_BASE_EXPORT mafString mafToString(int       d);
FTK_BASE_EXPORT mafString mafToString(long      d);
FTK_BASE_EXPORT mafString mafToString(long long d);
FTK_BASE_EXPORT mafString mafToString(float     d);
FTK_BASE_EXPORT mafString mafToString(double    d);

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
