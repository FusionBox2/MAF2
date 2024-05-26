/*=========================================================================

 Program: MAF2
 Module: mafStorageElement
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafObject.h"
#include "mafObjectFactory.h"
#include "mafStorable.h"
#include "mafStorageElement.h"
#include "mafParser.h"
#include "mafStorage.h"
#include "mafString.h"
#include "mafMatrix.h"
#include <vector>
#include <assert.h>


#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include "mafXMLParser.h"
#include "mafXMLString.h"

#include "mafMatrix.h"
#include "mafStorable.h"
#include "mafObjectFactory.h"

#include <vector>
#include <assert.h>
#include "stdio.h"

namespace
{
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* getDOMNode(void* element)
    {
        return reinterpret_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*>(element);
    }

    //------------------------------------------------------------------------------
    template <class T>
    void InternalStoreVectorN(mafStorageElementBuilder& element, T* comps, size_t num)
        //------------------------------------------------------------------------------
    {
        // Write all the elements into as a single 3-tupla  
        mafString elements;
        for (size_t i = 0; i < num; i++)
        {
            elements += mafToString(comps[i]) + _R(" ");
        }
        element.StoreText(elements);
    }
    //------------------------------------------------------------------------------
    template <class T>
    size_t InternalParseData(const mafString& text, T* vector, size_t size)
        //------------------------------------------------------------------------------
    {
#pragma message ("potentially hacky")
        std::istringstream instr(text.toStd());

        for (size_t i = 0; i < size; i++)
        {
            if (instr.eof())
                return i;

            instr >> vector[i];
        }

        return size;
    }
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreMatrix(mafMatrix& matrix) const
//------------------------------------------------------------------------------
{
  matrix.Zero();

  double *elem=*matrix.GetElements();
  mafString text_data;
  RestoreText(text_data);
  size_t parsedElems = 0;
  if (!text_data.empty())
	  parsedElems = InternalParseData(text_data, elem, 16);
  if (parsedElems == 16)
  {
    mafTimeStamp time_stamp;
    GetAttributeAsDouble(_R("TimeStamp"),time_stamp);
    matrix.SetTimeStamp(time_stamp);
    return MAF_OK; 
  }

  mafWarningMacro("Storage Parse Error while parsing <"<<GetName().GetCStr()<<"> element: wrong number of fields inside Storage element" );

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(double *comps,unsigned int num) const
//------------------------------------------------------------------------------
{
  mafString text_data;
  RestoreText(text_data);
  size_t parsedElems = 0;
  if (!text_data.empty())
	  parsedElems = InternalParseData(text_data, comps, num);
  if (parsedElems==num)
    return MAF_OK;

  mafWarningMacro("Storage Parse Error while parsing <"<<GetName().GetCStr()<<"> element: wrong number of fields inside Storage element." );

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(int *comps,unsigned int num) const
//------------------------------------------------------------------------------
{
  mafString text_data;
  RestoreText(text_data);
  size_t parsedElems = 0;
  if (!text_data.empty())
	  parsedElems = InternalParseData(text_data, comps, num);
  if (parsedElems==num)
    return MAF_OK;

  mafWarningMacro("Storage Parse Error while parsing <"<<GetName().GetCStr()<<"> element: wrong number of fields inside Storage element." );

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElementBuilder::StoreVectorN(double *comps,int num)
//------------------------------------------------------------------------------
{
  assert(comps);
  InternalStoreVectorN(*this, comps, num);
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafStorageElementBuilder::StoreVectorN(int *comps,int num)
//------------------------------------------------------------------------------
{
  assert(comps);
  InternalStoreVectorN(*this,comps,num);
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafStorageElementBuilder::StoreVectorN(const std::vector<double> &comps)
//------------------------------------------------------------------------------
{
  InternalStoreVectorN(*this,comps.data(),comps.size());
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafStorageElementBuilder::StoreVectorN(const std::vector<int> &comps)
//------------------------------------------------------------------------------
{
  InternalStoreVectorN(*this,comps.data(),comps.size());
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafStorageElementBuilder::StoreVectorN(const std::vector<mafString> &comps,const mafString& tag)
//------------------------------------------------------------------------------
{
  for (auto& elem : comps)
  {
    (*this)[tag].StoreText(elem);
  }
  return MAF_OK;
}
//------------------------------------------------------------------------------
mafStorageElement::mafStorageElement(void* element, mafParser *storage)
//------------------------------------------------------------------------------
{
  assert(storage); // no NULL storage is allowed
  m_Storage = storage;
  assert(element);
  m_DOMElement = element;
  auto child_element = getDOMNode(m_DOMElement)->getFirstChild();
  while (child_element)
  {
	  if (child_element->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
	  {
		  mafStorageElement child((XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*)child_element, GetStorage());
		  m_Children[child.GetName()].push_back(child);
	  }
	  child_element = child_element->getNextSibling();
  }
}
//------------------------------------------------------------------------------
mafString mafStorageElement::GetName() const
//------------------------------------------------------------------------------
{
	if (getDOMNode(m_DOMElement)->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return _R(mafXMLString(static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(getDOMNode(m_DOMElement))->getTagName()));
    return _R("");
}
//------------------------------------------------------------------------------
mafStorageElement::~mafStorageElement()
//------------------------------------------------------------------------------
{
  // remove pointers...
  m_Storage = NULL;
}

//------------------------------------------------------------------------------
mafStorageElementBuilder::mafStorageElementBuilder(void* element, mafParser* storage)
//------------------------------------------------------------------------------
{
	assert(storage); // no NULL storage is allowed
	m_Storage = storage;
	assert(element);
	m_DOMElement = element;
}
//------------------------------------------------------------------------------
mafStorageElementBuilder::~mafStorageElementBuilder()
//------------------------------------------------------------------------------
{
	// remove pointers...
	m_Storage = NULL;
}

mafStorageElementBuilder mafStorageElementBuilder::operator[](const mafString& name)
{
	//BuildChildrenMap();
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* child_element = getDOMNode(m_DOMElement)->getOwnerDocument()->createElement(mafXMLString(name.GetCStr()));
	getDOMNode(m_DOMElement)->appendChild(child_element);
	auto child = new mafStorageElementBuilder(child_element, GetStorage());
	//(*m_Children)[child->GetName()].push_back(child);
	return *child;
}

mafStorageElementBuilder mafStorageElementBuilder::operator()(const mafString& name)
{
    if (getDOMNode(m_DOMElement)->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
        throw 0;
	auto newAttr = getDOMNode(m_DOMElement)->getOwnerDocument()->createAttribute(mafXMLString(name.GetCStr()));
    static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(getDOMNode(m_DOMElement))->setAttributeNode(newAttr);
    return mafStorageElementBuilder(newAttr, GetStorage());
}

mafStorageElement mafStorageElement::operator[](const mafString& name) const
{
	auto it = m_Children.find(name);
    if (it == m_Children.end())
        throw 0;
	return it->second.front();
}

mafStorageElement mafStorageElement::operator()(const mafString& name) const
{
    if (getDOMNode(m_DOMElement)->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
        throw 0;
    return mafStorageElement(static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(getDOMNode(m_DOMElement))->getAttributeNode(mafXMLString(name.GetCStr())), GetStorage());
}

//------------------------------------------------------------------------------
std::vector<mafStorageElement> mafStorageElement::GetElementsByName(const mafString& name) const
//------------------------------------------------------------------------------
{
  auto it = m_Children.find(name);
  if (it != m_Children.end())
  {
      return it->second;
  }
  return {};
}

//------------------------------------------------------------------------------
int mafStorageElementBuilder::StoreVectorN(const std::vector<mafObject *> &vector,const mafString& items_name)
//------------------------------------------------------------------------------
{
  SetAttribute(_R("NumberOfItems"),mafToString((long)vector.size()));
  
  for (unsigned int i=0;i<vector.size();i++)
  {
    mafObject *object=vector[i];
    if (object)
    {
      if ((*this)[items_name].StoreObject(object) != MAF_OK)
      {
        mafErrorMacro("Failed to store object of type \""<<object->GetTypeName()<<"\" in vector of objects");
        return MAF_ERROR;
      }
    }
    else
    {
      mafWarningMacro("NULL object in a vector being stored");
    }
  }

  return MAF_OK;  
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(std::vector<mafObject *> &vector,const mafString& items_name) const
//------------------------------------------------------------------------------
{
      auto items = GetElementsByName(items_name);

      mafID numItems=-1;
      if (GetAttributeAsInteger(_R("NumberOfItems"),numItems) != MAF_OK)
      {
        mafWarningMacro("Warning while restoring vector of objects from element <"<<GetName().GetCStr() <<">: cannot find \"NumberOfItems\" attribute..." );
      }

      int num=0;
      bool fail=false;
      for (auto& item : items)
      {
        {
          mafObject* object = nullptr;
          if (item.RestoreObject(object) != MAF_OK)
          {
            fail=true;
            mafString type_name;
            item.GetAttribute(_R("Type"),type_name);
            mafWarningMacro("Error while restoring vector of objects from element <"<< GetName().GetCStr() <<">: cannot restore object from element <"<<item.GetName().GetCStr() <<">, object's Type=\""<<type_name.GetCStr()<<"\".");
            // try continue restoring other objects
            GetStorage()->SetErrorCode(mafStorage::IO_WRONG_OBJECT_TYPE);
          }
	      else
          {
            vector.push_back(object);
          }
          num++;
        }    
      }

      if (fail)
        return MAF_ERROR;

      // check if restored num of items is correct
      if (numItems>=0&&num!=numItems)
      {
        mafWarningMacro("Error while restoring <"<< GetName().GetCStr() <<"> element: wrong number of items in Objects vector");
        return MAF_ERROR;
      }

      return MAF_OK;
}

//------------------------------------------------------------------------------
int mafStorageElementBuilder::StoreObject(mafObject *object)
//------------------------------------------------------------------------------
{
  mafString type_name = _R(object->GetTypeName());
  assert(object);
  SetAttribute(_R("Type"), type_name);
  mafStorable* storable = dynamic_cast<mafStorable*>(object);
  if (storable)
  {
      storable->Store(*this);
	  return MAF_OK;
  }
  mafErrorMacro("Failed to store object of type \"" << type_name.GetCStr() << "\"");
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElementBuilder::StoreStorable(mafStorable* storable)
//------------------------------------------------------------------------------
{
  assert(storable);
  return storable->Store(*this);
}

//------------------------------------------------------------------------------
mafString mafStorageElement::UpgradeAttribute(const mafString& attribute) const
//------------------------------------------------------------------------------
{
  mafString att_name;
  /*mafString new_att_name;
  GetAttribute(attribute, att_name);
  if (att_name == _R("mafVMEItemScalar"))
  {
    new_att_name = _R("mafVMEItemScalarMatrix");
    SetAttribute(attribute, new_att_name);
    return new_att_name;
  }
  if (att_name.find(_R("mafVME")) != mafString::npos)
  {
    if (att_name == _R("mafVMEScalar"))
    {
      new_att_name = _R("mafVMEScalarMatrix");
      SetAttribute(attribute, new_att_name);
    }
    else
    {
      new_att_name = att_name;
    }
    mafStorageElement data_vector = FindNestedElement(_R("DataVector"));
    mafString item_type;
    if (data_vector && data_vector->GetAttribute(_R("ItemTypeName"), item_type))
    {
      if (item_type == _R("mafVMEItemScalar"))
      {
        data_vector->SetAttribute(_R("ItemTypeName"), _R("mafVMEItemScalarMatrix"));
      }
    }
    return new_att_name;
  }*/
  
  return att_name;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreObject(mafObject*& object) const
//------------------------------------------------------------------------------
{
  mafString type_name;

  if (GetAttribute(_R("Type"),type_name)==MAF_OK&&!type_name.empty())
  {
    if (m_Storage->NeedsUpgrade())
    {
      type_name = UpgradeAttribute(_R("Type"));
    }
    
    object=mafObjectFactory::CreateInstance(type_name.GetCStr());
    if (object)
    {
      try 
      {
        // items must be both a mafObject and mafStorable 
        mafStorable *restorable=dynamic_cast<mafStorable *>(object);
        
        if (restorable)
        {
          if (restorable->Restore(*this)==MAF_OK)
          {
            // if restored correctly 
            return MAF_OK;
          }
          else
          {
            mafErrorMacro("Problems restoring object of type "<<object->GetTypeName()<<" from element <"<<GetName().GetCStr() <<">");
          }
        }
        else
        {
          mafErrorMacro("Cannot restore object of type "<<object->GetTypeName()<<" from element <"<<GetName().GetCStr() <<"> since it's a not a restorable object");
        }
      }
      catch (std::bad_cast) 
      {
        mafErrorMacro("Cannot restore object of type "<<object->GetTypeName()<<" from element <"<<GetName().GetCStr() <<"> since it's a not a restorable object");
      }
      // release object memory
      object->Delete();     
    }    
    else
    {
      mafErrorMacro("Cannot restore object of type \""<<type_name.GetCStr()<<"\" from element <"<<GetName().GetCStr() <<"> since this object type is unknown.");
    }
  }
  else
  {

    mafErrorMacro("Cannot restore object from element <"<<GetName().GetCStr() <<"> since no 'Type' attribute is present");
  }

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreStorable(mafStorable *storable) const
//------------------------------------------------------------------------------
{
  return storable->Restore(*this);
}
//------------------------------------------------------------------------------
int mafStorageElementBuilder::StoreMatrix(const mafMatrix& matrix)
//------------------------------------------------------------------------------
{
  // Write all the 16 elements into as a single 16-tupla
  mafString elements;
  for (int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    { 
      elements += mafToString(matrix.GetElements()[i][j]) + _R(" ");
    }
    elements += _R("\n"); // cr for read-ability
  }

  StoreText(elements);

  // add also the timestamp as an attribute
  SetAttribute(_R("TimeStamp"),mafToString(matrix.GetTimeStamp()));
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafStorageElementBuilder::StoreDouble(const double &value)
//------------------------------------------------------------------------------
{
  return StoreText(mafToString(value));
}
//------------------------------------------------------------------------------
int mafStorageElementBuilder::StoreInteger(const int &value)
//------------------------------------------------------------------------------
{
  return StoreText(mafToString(value));
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreDouble(double &value) const
//------------------------------------------------------------------------------
{
   mafString tmp;
   if (RestoreText(tmp)==MAF_OK)
   {
     value=atof(tmp.GetCStr());
     return MAF_OK;
   }
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreInteger(int &value) const
//------------------------------------------------------------------------------
{
   mafString tmp;
   if (RestoreText(tmp)==MAF_OK)
   {
     value=atof(tmp.GetCStr());
     return MAF_OK;
   }
  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(std::vector<double> &comps) const
//------------------------------------------------------------------------------
{
  return RestoreVectorN(comps.data(), comps.size());
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(std::vector<int> &comps) const
//------------------------------------------------------------------------------
{
  return RestoreVectorN(comps.data(), comps.size());
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(std::vector<mafString> &comps,const mafString& tag) const
//------------------------------------------------------------------------------
{
 auto children = GetElementsByName(tag);

  // to be rewritten as a map access
  for (size_t i=0;i<children.size();i++)
  {
    children[i].RestoreText(comps[i]);
  }
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafStorageElement::GetAttributeAsDouble(const mafString& name,double &value) const
//------------------------------------------------------------------------------
{
  mafString tmp;
  if (GetAttribute(name,tmp) == MAF_OK)
  {
    value=atof(tmp.GetCStr());
    return MAF_OK;
  }
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::GetAttributeAsInteger(const mafString& name,mafID &value) const
//------------------------------------------------------------------------------
{
  mafString tmp;
  if (GetAttribute(name,tmp) == MAF_OK)
  {
    value=atof(tmp.GetCStr());
    return MAF_OK;
  }
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElementBuilder::SetAttribute(const mafString& name,const mafID value)
//------------------------------------------------------------------------------
{
  return SetAttribute(name,mafToString(value));
}
//------------------------------------------------------------------------------
int mafStorageElementBuilder::SetAttribute(const mafString& name,const double value)
//------------------------------------------------------------------------------
{
  return SetAttribute(name,mafToString(value));
}

//------------------------------------------------------------------------------
int mafStorageElementBuilder::SetAttribute(const mafString& name, const mafString& value)
//------------------------------------------------------------------------------
{
    if (getDOMNode(m_DOMElement)->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
    {
        static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(getDOMNode(m_DOMElement))->setAttribute(mafXMLString(name.GetCStr()), mafXMLString(value.GetCStr()));
        return MAF_OK;
    }
	return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::GetAttribute(const mafString& name, mafString& value) const
//------------------------------------------------------------------------------
{
	if (getDOMNode(m_DOMElement)->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return false;
	const XMLCh* xml_value = static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(getDOMNode(m_DOMElement))->getAttribute(mafXMLString(name.GetCStr()));
	if (xml_value)
	{
		value = _R(mafXMLString(xml_value));
		return MAF_OK;
	}
	return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElementBuilder::StoreText(const mafString& text)
//------------------------------------------------------------------------------
{
	XERCES_CPP_NAMESPACE_QUALIFIER DOMText* text_node = getDOMNode(m_DOMElement)->getOwnerDocument()->createTextNode(mafXMLString(text.GetCStr()));
	getDOMNode(m_DOMElement)->appendChild(text_node);
	return MAF_OK;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreText(mafString& buffer) const
//------------------------------------------------------------------------------
{
    if (getDOMNode(m_DOMElement)->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE)
    {
        buffer = _R(mafXMLString(getDOMNode(m_DOMElement)->getNodeValue()));
        return MAF_OK;
    }
	if (getDOMNode(m_DOMElement)->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
	{
		auto child_element = getDOMNode(m_DOMElement)->getFirstChild();
		while (child_element)
		{
			if (child_element->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TEXT_NODE)
			{
				buffer = _R(mafXMLString(child_element->getNodeValue()));
				return MAF_OK;
			}
			child_element = child_element->getNextSibling();
		}
		return MAF_OK;
	}
	return MAF_ERROR;
}
