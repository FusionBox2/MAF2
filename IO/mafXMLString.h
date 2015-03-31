/*=========================================================================

 Program: MAF2
 Module: mafXMLString
 Authors: Rick Parrish (rfmobile@swbell.net) adapted by Marco Petrone (CINECA)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafXMLString_h__
#define __mafXMLString_h__

#include "mafConfigure.h"


#include <xercesc/util/XMLString.hpp>

#ifdef XERCES_CPP_NAMESPACE_USE
// XERCES_CPP_NAMESPACE_USE
// SIL 12-apr-2006
// removed XERCES_CPP_NAMESPACE_USE and added XERCES_CPP_NAMESPACE_QUALIFIER where required
#endif

/** string type for converting const char * to and from XMLString types
  This is an internal string type to be used only in conjunction with XercecC XML library.
  This object is able to convert const char* to and from XMLStrings. This code has been
  adapted from example code "class XercesString" found in articles "Make the most of Xerces-C++"
  by Rick Parrish (rfmobile@swbell.net) that can be found at "www.ibm.com/developerworks/xml".
*/
class MAF_EXPORT mafXMLString
{
public:
	mafXMLString() : m_WStr(0L), m_CStr(NULL) { };
	mafXMLString(const char *str);
	mafXMLString(XMLCh *wstr);
	mafXMLString(const XMLCh *wstr);
	mafXMLString(const mafXMLString &copy);
	~mafXMLString();
	bool Append(const XMLCh *tail);
	bool Erase(const XMLCh *head, const XMLCh *tail);
	const XMLCh* Begin() const;
	const XMLCh* End() const;
	int Size() const;
  const char *GetCStr()const;
	XMLCh & operator [] (const int i);
	const XMLCh operator [] (const int i) const;
	operator const XMLCh * () const { return m_WStr; };
  operator const char * () {return GetCStr();}
protected:
  XMLCh *m_WStr;
  mutable char *m_CStr;
};

#endif
