/*=========================================================================

 Program: MAF2
 Module: mafObject
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafObject.h"
#include "mafIndent.h"
#include <ostream>
#include <map>
#include <string>

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
/** This class is used as a dictionary to inference event IDs from class names. */
class mafObjectDictionaryType
{
  public:
  static std::map<std::string,mafID> *m_TypeIDs;

  mafObjectDictionaryType() {if (m_TypeIDs==NULL) m_TypeIDs=new std::map<std::string,mafID>;}
  ~mafObjectDictionaryType() {if (m_TypeIDs) delete m_TypeIDs;} // this is to allow memory deallocation
}; 

mafCxxTypeI0Macro(mafObject)

#ifndef _DEBUG
  #ifdef _WIN32
  //------------------------------------------------------------------------------
  // avoid dll boundary problems
  void* mafObject::operator new(size_t nSize)
  //------------------------------------------------------------------------------
  {
    void* p=malloc(nSize);
    return p;
  }

  //------------------------------------------------------------------------------
  void mafObject::operator delete( void *p )
  //------------------------------------------------------------------------------
  {
    free(p);
  }
  #endif 
#endif

//------------------------------------------------------------------------------
mafObject::mafObject():m_HeapFlag(0)
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mafObject::~mafObject()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mafObject::Print(std::ostream &os, const int indent) const
//------------------------------------------------------------------------------
{
  os << mafIndent(indent) << "Object Type Name: " << GetTypeName() << std::endl;
}
