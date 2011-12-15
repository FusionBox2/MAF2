/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuXMLDOMElement.h,v $
  Language:  C++
  Date:      $Date: 2011-12-15 10:40:31 $
  Version:   $Revision: 1.4.4.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmuXMLDOMElement_h__
#define __mmuXMLDOMElement_h__

#include "mafUtility.h"


#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/impl/DOMWriterImpl.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#ifdef XERCES_CPP_NAMESPACE_USE
// XERCES_CPP_NAMESPACE_USE
// SIL 12-apr-2006
// removed XERCES_CPP_NAMESPACE_USE and added XERCES_CPP_NAMESPACE_QUALIFIER where required
#endif

/** mmuXMLDOMElement utility class for PIMPL of XML DOMElement types.
 */  
//------------------------------------------------------------------------------
// mmuXMLDOMElement
//------------------------------------------------------------------------------
/** PIMPL class to hide XercesC specific XML DOM objects. 
  This class is internally used by mafXMLElement for accessing XML DOM objects.
*/
class MAF_EXPORT mmuXMLDOMElement: public mafUtility
{
public:
  /** constructor */
  mmuXMLDOMElement( XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *element=NULL): m_XMLElement(element) {}

  XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *m_XMLElement; ///< XML element wrapped by this object 
};

#endif // _mmuXMLDOMElement_h_
