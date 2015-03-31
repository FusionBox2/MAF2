/*=========================================================================

 Program: MAF2
 Module: mafXMLElement
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafXMLElement_h__
#define __mafXMLElement_h__

#include "mafStorageElement.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafXMLParser;
class mafXMLString;
class mmuXMLDOMElement;

/** concrete implementation of mafStorageElement as an XML-DOM element
  @sa mafXMLStorage
 */  
class MAF_EXPORT mafXMLElement : public mafStorageElement
{
public:
  mafXMLElement(mmuXMLDOMElement *element,mafXMLElement *parent,mafParser *storage);
  virtual ~mafXMLElement();

  /** Store a generic text into an XML document */
  virtual int StoreText(const mafString& text);

  /** Restore a generic text string from an XML document */
  virtual int RestoreText(mafString &buffer);

  /** Create a new XML child element and return its pointer.  */
  virtual mafStorageElement *AppendChild(const mafString& name);

  /** 
    Append an XML attribute to this element. Attribute 'name' and
    'value' must be passed as argument. This utility function takes care
    of string conversion problems. */
  virtual void SetAttribute(const mafString& name,const mafString& value);

  /** 
    Return find an attribute given its name and return its value.
    Return false if not found. This function takes care of string
    conversion problems. */
  virtual bool GetAttribute(const mafString& name,mafString &value);

protected:
  /** Return the list of children. The list is created from DOM-Tree at the first access during restoring. */ 
  virtual ChildrenVector &GetChildrenList();

  /** return DOM-XML element stored inside this mafXMLElement */
  mmuXMLDOMElement *GetXMLElement();

  mmuXMLDOMElement *m_DOMElement; ///< XML element wrapped by this object (USING PIMPL due to Internal Compile errors of VS7)

};
#endif // _mafXMLElement_h_

