#pragma once

#include "ftkConfigure.h"

#include "mafDefines.h"

#include "ftk/Base/String.h"

using mafString = base::String;

std::string mafStringToStd(const mafString& str);
mafString mafStdToString(const std::string& str);


#ifdef MAF_USE_WX
FTK_BASE_EXPORT mafString mafWxToString(const wxString& str);
FTK_BASE_EXPORT wxString mafStringToWx(const mafString& str);
#endif

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
