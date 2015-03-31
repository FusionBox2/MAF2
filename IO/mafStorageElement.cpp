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

//------------------------------------------------------------------------------
template <class T>
void InternalStoreVectorN(mafStorageElement *element,T *comps,int num,const char *name)
//------------------------------------------------------------------------------
{
  assert(name);

  // Write all the elements into as a single 3-tupla  
  mafString elements;
  for (int i=0;i<num;i++)
  { 
    elements << mafString(comps[i]) << " ";
  }
  mafStorageElement *vector_node=element->AppendChild(name);
  vector_node->StoreText(elements);
}
//------------------------------------------------------------------------------
template <class T>
void InternalStoreVectorN(mafStorageElement *element,const std::vector<T> &comps,int num,const char *name)
//------------------------------------------------------------------------------
{
  assert(name);

  // Write all the elements into as a single 3-tupla  
  mafString elements;
  for (int i=0;i<num;i++)
  { 
    elements << mafString(comps[i]) << " ";
  }

  mafStorageElement *vector_node=element->AppendChild(name);
  vector_node->StoreText(elements);
}
//------------------------------------------------------------------------------
template <class T>
int InternalParseData(const mafString& text,T *vector,int size)
//------------------------------------------------------------------------------
{
  std::istringstream instr(text.GetCStr());

  for (int i=0;i<size;i++)
  {
    if (instr.eof())
      return i;

    instr>>vector[i];
  }

  return size;
}

//------------------------------------------------------------------------------
template <class T>
int InternalParseData(const mafString& text,std::vector<T> &vector,int size)
//------------------------------------------------------------------------------
{
  std::istringstream instr(text.GetCStr());

  for (int i=0;i<size;i++)
  {
    if (instr.eof())
      return i;

    T tmp;
    instr>>tmp;
    vector[i]=tmp;
  }

  return size;
}

//------------------------------------------------------------------------------
int mafStorageElement::ParseData(std::vector<double> &vector,int size)
//------------------------------------------------------------------------------
{
  mafString text_data;
  RestoreText(text_data);
  if (text_data.IsEmpty())
    return 0;
  return InternalParseData(text_data,vector,size);
}
//------------------------------------------------------------------------------
int mafStorageElement::ParseData(std::vector<int> &vector,int size)
//------------------------------------------------------------------------------
{
  mafString text_data;
  RestoreText(text_data);
  if (text_data.IsEmpty())
    return 0;
  return InternalParseData(text_data,vector,size);
  return 0;
}

//------------------------------------------------------------------------------
int mafStorageElement::ParseData(double *vector,int size)
//------------------------------------------------------------------------------
{
  mafString text_data;
  RestoreText(text_data);
  if (text_data.IsEmpty())
    return 0;
  return InternalParseData(text_data,vector,size);
}
//------------------------------------------------------------------------------
int mafStorageElement::ParseData(int *vector,int size)
//------------------------------------------------------------------------------
{
  mafString text_data;
  RestoreText(text_data);
  if (text_data.IsEmpty())
    return 0;
  return InternalParseData(text_data,vector,size);
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreMatrix(mafMatrix *matrix)
//------------------------------------------------------------------------------
{
  assert(matrix);

  matrix->Zero();

  double *elem=*matrix->GetElements();
  if (this->ParseData(elem,16)==16)
  {
    mafTimeStamp time_stamp;
    this->GetAttributeAsDouble("TimeStamp",time_stamp);
    matrix->SetTimeStamp(time_stamp);
    return MAF_OK; 
  }

  mafWarningMacro("Storage Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside Storage element" );

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(double *comps,unsigned int num)
//------------------------------------------------------------------------------
{
  if (this->ParseData(comps,num)==num)
    return MAF_OK;

  mafWarningMacro("Storage Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside Storage element." );

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(int *comps,unsigned int num)
//------------------------------------------------------------------------------
{
  if (this->ParseData(comps,num)==num)
    return MAF_OK;

  mafWarningMacro("Storage Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside Storage element." );

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(std::vector<double> &comps,unsigned int num)
//------------------------------------------------------------------------------
{
  if (this->ParseData(comps,num)==num)
    return MAF_OK;

  mafWarningMacro("Storage Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside Storage element." );

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(std::vector<int> &comps,unsigned int num)
//------------------------------------------------------------------------------
{
  if (this->ParseData(comps,num)==num)
    return MAF_OK;

  mafWarningMacro("Storage Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside Storage element." );

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::StoreVectorN(const mafString& name,double *comps,int num)
//------------------------------------------------------------------------------
{
  assert(comps);
  InternalStoreVectorN(this,comps,num,name);
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafStorageElement::StoreVectorN(const mafString& name,int *comps,int num)
//------------------------------------------------------------------------------
{
  assert(comps);
  InternalStoreVectorN(this,comps,num,name);
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafStorageElement::StoreVectorN(const mafString& name,const std::vector<double> &comps,int num)
//------------------------------------------------------------------------------
{
  InternalStoreVectorN(this,comps,num,name);
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafStorageElement::StoreVectorN(const mafString& name,const std::vector<int> &comps,int num)
//------------------------------------------------------------------------------
{
  InternalStoreVectorN(this,comps,num,name);
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafStorageElement::StoreVectorN(const mafString& name,const std::vector<mafString> &comps,int num,const mafString& tag)
//------------------------------------------------------------------------------
{
  assert(name);
  assert(tag);

  mafStorageElement *subelement = AppendChild(name);
  for (int i=0;i<num;i++)
  {
    subelement->StoreText(tag,comps[i]);
  }
  return MAF_OK;
}
//------------------------------------------------------------------------------
mafStorageElement::mafStorageElement(mafStorageElement *parent,mafParser *storage)
//------------------------------------------------------------------------------
{
  assert(storage); // no NULL storage is allowed
  m_Storage = storage;
  m_Children = NULL;
}
//------------------------------------------------------------------------------
mafStorageElement::~mafStorageElement()
//------------------------------------------------------------------------------
{
  // remove pointers...
  m_Storage = NULL;

  if (m_Children)
  {
    // remove all child nodes
    for (unsigned int i=0;i<m_Children->size();i++)
    {
      delete (*m_Children)[i];
    }
  
    cppDEL(m_Children);
  }  
}

//------------------------------------------------------------------------------
mafStorageElement *mafStorageElement::FindNestedElement(const mafString& name)
//------------------------------------------------------------------------------
{
  // force children list creation
  const ChildrenVector &children=GetChildren();
  
  // to be rewritten as a map access
  for (unsigned int i=0;i<children.size();i++)
  {
    mafStorageElement *node=children[i];
    if (name==node->GetName())
      return node;
  }

  return NULL;
}

//------------------------------------------------------------------------------
bool mafStorageElement::GetNestedElementsByName(const mafString& name,std::vector<mafStorageElement *> &list)
//------------------------------------------------------------------------------
{
  // force children list creation
  const ChildrenVector &children=GetChildren();
  list.clear();
  
  // to be rewritten as a map access
  for (unsigned int i=0;i<children.size();i++)
  {
    mafStorageElement *node=children[i];
    if (name==node->GetName())
    {
      list.push_back(node);
    }
  }

  return list.size()>0;

}

//------------------------------------------------------------------------------
int mafStorageElement::StoreObjectVector(const mafString& name,const std::vector<mafObject *> &vector,const mafString& items_name)
//------------------------------------------------------------------------------
{
  assert(name);

  // create sub node for storing the vector
  mafStorageElement *vector_node = AppendChild(name);
  vector_node->SetAttribute("NumberOfItems",mafString(vector.size()));
  
  for (unsigned int i=0;i<vector.size();i++)
  {
    mafObject *object=vector[i];
    if (object)
    {
      if (vector_node->StoreObject(items_name,object)==NULL)
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
int mafStorageElement::RestoreObjectVector(const mafString& name,std::vector<mafObject *> &vector,const mafString& items_name)
//------------------------------------------------------------------------------
{
  assert(name);

  mafStorageElement *subnode=FindNestedElement(name);
  if (subnode)
  {
    return RestoreObjectVector(subnode,vector,items_name);
  }
  
  mafWarningMacro("Error while restoring <"<<GetName()<<"> element: cannot find nested element <"<<name<<">" );
  
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreObjectVector(mafStorageElement *subnode,std::vector<mafObject *> &vector,const mafString& items_name)
//------------------------------------------------------------------------------
{
  assert(subnode);

  const ChildrenVector &items = subnode->GetChildren();

  mafID numItems=-1;
  if (!subnode->GetAttributeAsInteger("NumberOfItems",numItems))
  {
    mafWarningMacro("Warning while restoring vector of objects from element <"<<GetName()<<">: cannot find \"NumberOfItems\" attribute..." );
  }

  int num=0;
  bool fail=false;
  for (unsigned int i=0;i<items.size();i++)
  {
    mafStorageElement *item=items[i];
    if (items_name == item->GetName())
    {
      mafObject *object=item->RestoreObject();
      if (object)
      {
        vector.push_back(object);
      }
      else
      {
        fail=true;
        mafString type_name;
        item->GetAttribute("Type",type_name);
        mafWarningMacro("Error while restoring vector of objects from element <"<<GetName()<<">: cannot restore object from element <"<<item->GetName()<<">, object's Type=\""<<type_name.GetCStr()<<"\".");
        // try continue restoring other objects
        GetStorage()->SetErrorCode(mafStorage::IO_WRONG_OBJECT_TYPE);
      }
      num++;
    }    
  }

  if (fail)
    return MAF_ERROR;

  // check if restored num of items is correct
  if (numItems>=0&&num!=numItems)
  {
    mafWarningMacro("Error while restoring <"<<GetName()<<"> element: wrong number of items in Objects vector");
    return MAF_ERROR;
  }

  return MAF_OK;
}

//------------------------------------------------------------------------------
mafStorageElement *mafStorageElement::StoreObject(const mafString& name,mafStorable *storable, const mafString& type_name)
//------------------------------------------------------------------------------
{
  assert(storable);
  if (storable->IsStorable())
  {
    mafStorageElement *element=AppendChild(name);
    if (element)
    {
      element->SetAttribute("Type",type_name);

      if (storable->Store(element)==MAF_OK)
      {
        return element;
      }
    }
    mafErrorMacro("Failed to store object of type \""<<type_name<<"\"");
  }
  
  return NULL;
}
//------------------------------------------------------------------------------
mafStorageElement *mafStorageElement::StoreObject(const mafString& name,mafObject *object)
//------------------------------------------------------------------------------
{
  assert(name);
  assert(object);
  
  try 
  {
    // items must be both a mafStorable object 
    mafStorable *storable=dynamic_cast<mafStorable *>(object);
    //mafStorable *storable=mafStorable::SafeDownCast(object);
    
    if (storable)
    {
      return StoreObject(name,storable,object->GetTypeName());
    }
    else
    {
      mafWarningMacro("NULL object in a vector being stored");
    }
  } 
  catch (std::bad_cast) 
  { 
    mafErrorMacro("Cannot store the given object because it's not a storable object");

  }
  return NULL;
}
//------------------------------------------------------------------------------
mafString mafStorageElement::UpgradeAttribute(const mafString& attribute)
//------------------------------------------------------------------------------
{
  mafString att_name;
  mafString new_att_name = "";
  GetAttribute(attribute,att_name);
  if (att_name.Equals("mafVMEItemScalar"))
  {
    new_att_name = "mafVMEItemScalarMatrix";
    SetAttribute(attribute, new_att_name);
    return new_att_name;
  }
  if (att_name.FindFirst("mafVME") != -1)
  {
    if (att_name.Equals("mafVMEScalar"))
    {
      new_att_name = "mafVMEScalarMatrix";
      SetAttribute(attribute, new_att_name);
    }
    else
    {
      new_att_name = att_name;
    }
    mafStorageElement *data_vector = FindNestedElement("DataVector");
    mafString item_type;
    if (data_vector && data_vector->GetAttribute("ItemTypeName", item_type))
    {
      if (item_type.Equals("mafVMEItemScalar"))
      {
        data_vector->SetAttribute("ItemTypeName", "mafVMEItemScalarMatrix");
      }
    }
    return new_att_name;
  }
  
  return att_name;
}
//------------------------------------------------------------------------------
mafObject *mafStorageElement::RestoreObject()
//------------------------------------------------------------------------------
{
  mafString type_name;

  if (GetAttribute("Type",type_name)&&!type_name.IsEmpty())
  {
    if (m_Storage->NeedsUpgrade())
    {
      type_name = UpgradeAttribute("Type");
    }
    
    mafObject *object=mafObjectFactory::CreateInstance(type_name);
    if (object)
    {
      try 
      {
        // items must be both a mafObject and mafStorable 
        mafStorable *restorable=dynamic_cast<mafStorable *>(object);
        
        if (restorable)
        {
          if (restorable->Restore(this)==MAF_OK)
          {
            // if restored correctly 
            return object;
          }
          else
          {
            mafErrorMacro("Problems restoring object of type "<<object->GetTypeName()<<" from element <"<<GetName()<<">");
          }
        }
        else
        {
          mafErrorMacro("Cannot restore object of type "<<object->GetTypeName()<<" from element <"<<GetName()<<"> since it's a not a restorable object");
        }
      }
      catch (std::bad_cast) 
      {
        mafErrorMacro("Cannot restore object of type "<<object->GetTypeName()<<" from element <"<<GetName()<<"> since it's a not a restorable object");
      }
      // release object memory
      object->Delete();     
    }    
    else
    {
      mafErrorMacro("Cannot restore object of type \""<<type_name.GetCStr()<<"\" from element <"<<GetName()<<"> since this object type is unknown.");
    }
  }
  else
  {

    mafErrorMacro("Cannot restore object from element <"<<GetName()<<"> since no 'Type' attribute is present");
  }

  return NULL;
}

//------------------------------------------------------------------------------
mafObject *mafStorageElement::RestoreObject(const mafString& name)
//------------------------------------------------------------------------------
{
  mafStorageElement *element=FindNestedElement(name);
  return (element)?element->RestoreObject():NULL;
}


//------------------------------------------------------------------------------
int mafStorageElement::RestoreObject(const mafString& name,mafStorable * object)
//------------------------------------------------------------------------------
{
  mafStorageElement *element=FindNestedElement(name);

  if (element)
  {
    return object->Restore(element);
  }
  
  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::StoreText(const mafString& name, const mafString& text)
//------------------------------------------------------------------------------
{
  assert(text);
  assert(name);

  mafStorageElement *text_node=AppendChild(name);
  text_node->StoreText(text);
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafStorageElement::StoreMatrix(const mafString& name,const mafMatrix *matrix)
//------------------------------------------------------------------------------
{
  assert(matrix);
  assert(name);

  // Write all the 16 elements into as a single 16-tupla
  mafString elements;
  for (int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    { 
      elements << mafString(matrix->GetElements()[i][j]) << " ";
    }
    elements << "\n"; // cr for read-ability
  }

  mafStorageElement *matrix_node=AppendChild(name);
  matrix_node->StoreText(elements);

  // add also the timestamp as an attribute
  matrix_node->SetAttribute("TimeStamp",mafString(matrix->GetTimeStamp()));
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafStorageElement::StoreDouble(const mafString& name,const double &value)
//------------------------------------------------------------------------------
{
  return StoreText(name,mafString(value));
}
//------------------------------------------------------------------------------
int mafStorageElement::StoreInteger(const mafString& name,const int &value)
//------------------------------------------------------------------------------
{
  return StoreText(name,mafString(value));
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreDouble(const mafString& name,double &value)
//------------------------------------------------------------------------------
{
  mafString tmp;

  if (RestoreText(name,tmp)==MAF_OK)
  {
    value=atof(tmp);
    return MAF_OK;
  }
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreDouble(double &value)
//------------------------------------------------------------------------------
{
  mafString tmp;
  if (RestoreText(tmp)==MAF_OK)
  {
    value=atof(tmp);
    return MAF_OK;
  }

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreInteger(int &value)
//------------------------------------------------------------------------------
{
  mafString tmp;
  if (RestoreText(tmp)==MAF_OK)
  {
    value=atof(tmp);
    return MAF_OK;
  }

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreInteger(const mafString& name,int &value)
//------------------------------------------------------------------------------
{
  mafString tmp;
  if (RestoreText(name,tmp)==MAF_OK)
  {
    value=atof(tmp);
    return MAF_OK;
  }

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreMatrix(const mafString& name,mafMatrix *matrix)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreMatrix(matrix);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested Storage element <"<<name<<">" );

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(const mafString& name,double *comps,unsigned int num)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested Storage element <"<<name<<">" );

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(const mafString& name,int *comps,unsigned int num)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested Storage element <"<<name<<">" );

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(const mafString& name,std::vector<double> &comps,unsigned int num)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested Storage element <"<<name<<">" );

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(const mafString& name,std::vector<int> &comps,unsigned int num)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested Storage element <"<<name<<">" );

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(const mafString& name,std::vector<mafString> &comps,unsigned int num,const mafString& tag)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num,tag);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested Storage element <"<<name<<">" );

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(std::vector<mafString> &comps,unsigned int num,const mafString& tag)
//------------------------------------------------------------------------------
{
  assert(tag);

  mafString tag_name=tag;

  // force children list creation
  const ChildrenVector &children=this->GetChildren();

  // to be rewritten as a map access
  for (unsigned int i=0;i<children.size();i++)
  {
    mafStorageElement *item_node=children[i];
    if (tag_name==item_node->GetName())
    {
      item_node->RestoreText(comps[i]);
    }
    else
    {
      mafWarningMacro("Storage Parse Error while parsing <"<<GetName()<<"> item_node: wrong sub-element inside nested Storage element <"<<(tag_name.GetCStr())<<">" );
      return MAF_ERROR;
    }
  }
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreText(const mafString& name,mafString &buffer)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreText(buffer);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested Storage element <"<<name<<">" );

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
bool mafStorageElement::GetAttributeAsDouble(const mafString& name,double &value)
//------------------------------------------------------------------------------
{
  mafString tmp;
  if (GetAttribute(name,tmp))
  {
    value=atof(tmp);
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
bool mafStorageElement::GetAttributeAsInteger(const mafString& name,mafID &value)
//------------------------------------------------------------------------------
{
  mafString tmp;
  if (GetAttribute(name,tmp))
  {
    value=atof(tmp);
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
void mafStorageElement::SetAttribute(const mafString& name,const mafID value)
//------------------------------------------------------------------------------
{
  SetAttribute(name,mafString(value));
}
//------------------------------------------------------------------------------
void mafStorageElement::SetAttribute(const mafString& name,const double value)
//------------------------------------------------------------------------------
{
  SetAttribute(name,mafString(value));
}