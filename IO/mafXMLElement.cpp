/*=========================================================================

 Program: MAF2
 Module: mafXMLElement
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mmuXMLDOMElement.h"
#include "mmuXMLDOM.h"
#include "mafXMLElement.h"

#include "mafXMLStorage.h"
#include "mafXMLString.h"

#include "mafMatrix.h"
#include "mafStorable.h"
#include "mafObjectFactory.h"

#include <vector>
#include <assert.h>
#include "stdio.h"

//------------------------------------------------------------------------------
mafXMLElement::mafXMLElement(mmuXMLDOMElement *element,mafXMLElement *parent,mafParser *storage) :
  mafStorageElement(parent,storage)
//------------------------------------------------------------------------------
{
  assert(storage);
  assert(element);
  assert(element->m_XMLElement);
  m_DOMElement = element;
  m_Name = mafXMLString(element->m_XMLElement->getTagName());
}

//------------------------------------------------------------------------------
mafXMLElement::~mafXMLElement()
//------------------------------------------------------------------------------
{
  // the XML element is destroyed by its creator (the DOMDocument)
  cppDEL(m_DOMElement);
}
//------------------------------------------------------------------------------
mmuXMLDOMElement *mafXMLElement::GetXMLElement()
//------------------------------------------------------------------------------
{
  return m_DOMElement;
}
//------------------------------------------------------------------------------
mafStorageElement::ChildrenVector &mafXMLElement::GetChildrenList()
//------------------------------------------------------------------------------
{
  if (!m_Children)
  {
    // create and fill in new children list with element nodes
    m_Children = new ChildrenVector;
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *children=m_DOMElement->m_XMLElement->getChildNodes();
    
	int length = children->getLength();
	for (unsigned int i = 0; i<length;i++)
    {
      XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *child_element=children->item(i);
      if (child_element->getNodeType()==XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
      {
        mafXMLElement *child=new mafXMLElement(
			new mmuXMLDOMElement((XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *)child_element),this,GetStorage());
        m_Children->push_back(child);
      }
    }
  }
  return *m_Children;
}
//------------------------------------------------------------------------------
mafStorageElement *mafXMLElement::AppendChild(const mafString& name)
//------------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *child_element=GetXMLElement()->m_XMLElement->getOwnerDocument()->createElement(mafXMLString(name));
  m_DOMElement->m_XMLElement->appendChild(child_element);
  mafXMLElement *child=new mafXMLElement(new mmuXMLDOMElement(child_element),this,GetStorage());
  GetChildrenList().push_back(child);
  return child;
}
//------------------------------------------------------------------------------
void mafXMLElement::SetAttribute(const mafString& name,const mafString& value)
//------------------------------------------------------------------------------
{
  assert(name);
  assert(value);
  
  m_DOMElement->m_XMLElement->setAttribute(mafXMLString(name),mafXMLString(value));
}
//------------------------------------------------------------------------------
bool mafXMLElement::GetAttribute(const mafString& name, mafString &value)
//------------------------------------------------------------------------------
{
  assert(name);
  const XMLCh *xml_value=m_DOMElement->m_XMLElement->getAttribute(mafXMLString(name));
  if (xml_value)
  {
    value=mafXMLString(xml_value);
    return true;
  }
  return false; 
}
//------------------------------------------------------------------------------
int mafXMLElement::StoreText(const mafString& text)
//------------------------------------------------------------------------------
{
  assert(text);
  XERCES_CPP_NAMESPACE_QUALIFIER DOMText *text_node=GetXMLElement()->m_XMLElement->getOwnerDocument()->createTextNode(mafXMLString(text));
  m_DOMElement->m_XMLElement->appendChild(text_node);
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafXMLElement::RestoreText(mafString &buffer)
//------------------------------------------------------------------------------
{
  buffer=mafXMLString(this->GetXMLElement()->m_XMLElement->getTextContent());

  return MAF_OK;
}


