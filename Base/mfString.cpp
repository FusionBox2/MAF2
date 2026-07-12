#include "Base/mfString.h"

#ifdef MAF_USE_WX
mafString mafWxToString(const wxString& str)
{
#ifndef FUNCTIONAL_STRBUF
	return str.c_str();
#else
	return base::StrBuf::MakeBuf(str.c_str());
#endif
}

wxString mafStringToWx(const mafString& str)
{
	return str.c_str().GetBuf();
}
#endif

mafString mafToString(int d)
{
	return mafString::Format(_R("%d"), d);
}

mafString mafToString(long d)
{
	return mafString::Format(_R("%ld"), d);
}

mafString mafToString(long long d)
{
	return mafString::Format(_R("%lld"), d);
}

mafString mafToString(float d)
{
	return mafString::Format(_R("%.7g"), d);
}

mafString mafToString(double d)
{
	return mafString::Format(_R("%.16g"), d);
}

template <class T>
mafString InternalStoreVectorN(T* comps, size_t num)
{
	// Write all the elements into as a single 3-tupla  
	mafString elements;
	for (size_t i = 0; i < num; i++)
	{
		elements += mafToString(comps[i]) + _R(" ");
	}
	return elements;
}

mafString mafToString(double* comps, int num)
{
	assert(comps);
	return InternalStoreVectorN(comps, num);
}

mafString mafToString(int* comps, int num)
{
	assert(comps);
	return InternalStoreVectorN(comps, num);
}

mafString mafToString(const std::vector<double>& comps)
{
	return InternalStoreVectorN(comps.data(), comps.size());
}

mafString mafToString(const std::vector<int>& comps)
{
	return InternalStoreVectorN(comps.data(), comps.size());
}

mafMessageBuf _M(const mafString& s)
{
	return mafMessageBuf(s.GetCStr());
}


template <class T>
size_t InternalParseData(const mafString& text, T* vector, size_t size)
{
#pragma message ("potentially hacky")
	std::istringstream instr(mafStringToStd(text));

	for (size_t i = 0; i < size; i++)
	{
		if (instr.eof())
			return i;

		instr >> vector[i];
	}

	return size;
}

void mafParseVector(const mafString& str, double* comps, unsigned int num)
{
	size_t parsedElems = 0;
	if (!str.empty())
		parsedElems = InternalParseData(str, comps, num);
	if (parsedElems == num)
		return;
	//mafWarningMacro("Storage Parse Error while parsing <" << GetName().GetCStr() << "> element: wrong number of fields inside Storage element.");
}

void mafParseVector(const mafString& str, int* comps, unsigned int num)
{
	size_t parsedElems = 0;
	if (!str.empty())
		parsedElems = InternalParseData(str, comps, num);
	if (parsedElems == num)
		return;
	//mafWarningMacro("Storage Parse Error while parsing <"<<GetName().GetCStr()<<"> element: wrong number of fields inside Storage element." );
}

void mafParseVector(const mafString& str, std::vector<double>& comps)
{
	mafParseVector(str, comps.data(), comps.size());
}

void mafParseVector(const mafString& str, std::vector<int>& comps)
{
	mafParseVector(str, comps.data(), comps.size());
}

std::string mafStringToStd(const mafString& str)
{
	return base::StringToStd(str);
}

mafString mafStdToString(const std::string& str)
{
	return base::StdToString(str);
}

