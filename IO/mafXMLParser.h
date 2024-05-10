/*=========================================================================

 Program: MAF2
 Module: mafXMLStorage
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafXMLStorage_h__
#define __mafXMLStorage_h__

#include "mafParser.h"

/** Concrete implementation of Storage object using Xerces-C for storing in XML.
  This is a concrete implementation of storage object for storing XML documents
  by means of Xerces-C library (http://xml.apache.org/xerces-c/).
  This class also defines a function to access to XML/Xerces-C specific objects, 
  stored into a PIMPL class (mmuXMLDOM).
  @sa mafStorage mafXMLElement mmuXMLDOM
  @todo
    - remove "IncludeWX.h" inclusion from .cpp
    - add support for NULL destination URL
*/  
class MAF_EXPORT mafXMLParser: public mafParser
{
public:  
  enum XML_IO_ERRORS {IO_XML_PARSE_ERROR=mafParser::IO_LAST_ERROR,IO_DOM_XML_ERROR,IO_XML_PARSER_INTERNAL_ERROR, IO_LAST_ERROR};

  mafXMLParser(const mafString& filetype, const mafString& version);
  virtual ~mafXMLParser();

protected:
  /** This is called by Store() and must be reimplemented by subclasses */
  virtual int InternalStore();

  /** This is called by Restore() and must be reimplemented by subclasses */
  virtual int InternalRestore();
};
#endif // _mafXMLStorage_h_
