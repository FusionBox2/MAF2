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
#include <vector>
#include <assert.h>

//------------------------------------------------------------------------------
mafStorageElement::mafStorageElement(mafStorageElement *parent,mafParser *storage)
//------------------------------------------------------------------------------
{
  assert(storage); // no NULL storage is allowed
  m_Storage = storage;
  m_Parent = parent;
  //m_Children = new std::vector<mafStorageElement>;
  m_Children = NULL;
}
//------------------------------------------------------------------------------
mafStorageElement::~mafStorageElement()
//------------------------------------------------------------------------------
{
  // remove pointers...
  m_Storage = NULL;
  m_Parent = NULL;

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
mafStorageElement *mafStorageElement::FindNestedElement(const char *name)
//------------------------------------------------------------------------------
{
  mafString node_name(name); // no memory copy, thanks mafString :-)

  // force children list creation
  ChildrenVector &children=GetChildren();
  
  // to be rewritten as a map access
  for (unsigned int i=0;i<children.size();i++)
  {
    mafStorageElement *node=children[i];
    if (node_name==node->GetName())
      return node;
  }

  return NULL;
}

//------------------------------------------------------------------------------
bool mafStorageElement::GetNestedElementsByName(const char *name,std::vector<mafStorageElement *> &list)
//------------------------------------------------------------------------------
{
  mafString node_name(name); // no memory copy, thanks mafString :-)

  // force children list creation
  ChildrenVector &children=GetChildren();
  list.clear();
  
  // to be rewritten as a map access
  for (unsigned int i=0;i<children.size();i++)
  {
    mafStorageElement *node=children[i];
    if (node_name==node->GetName())
    {
      list.push_back(node);
    }
  }

  return list.size()>0;

}

//------------------------------------------------------------------------------
int mafStorageElement::StoreObjectVector(const char *name,const std::vector<mafObject *> &vector,const char *items_name)
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
int mafStorageElement::RestoreObjectVector(const char *name,std::vector<mafObject *> &vector,const char *items_name)
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
int mafStorageElement::RestoreObjectVector(mafStorageElement *subnode,std::vector<mafObject *> &vector,const char *items_name)
//------------------------------------------------------------------------------
{
  assert(subnode);

  ChildrenVector &items = subnode->GetChildren();

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
    if (mafString::Equals(items_name,item->GetName()))
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
mafStorageElement *mafStorageElement::StoreObject(const char *name,mafStorable *storable, const char *type_name)
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
mafStorageElement *mafStorageElement::StoreObject(const char *name,mafObject *object)
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
mafString mafStorageElement::UpgradeAttribute(const char *attribute)
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
mafObject *mafStorageElement::RestoreObject(const char *name)
//------------------------------------------------------------------------------
{
  mafStorageElement *element=FindNestedElement(name);
  return (element)?element->RestoreObject():NULL;
}


//------------------------------------------------------------------------------
int mafStorageElement::RestoreObject(const char *name,mafStorable * object)
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
int mafStorageElement::StoreDouble(const char *name,const double &value)
//------------------------------------------------------------------------------
{
  return StoreText(name,mafString(value));
}
//------------------------------------------------------------------------------
int mafStorageElement::StoreInteger(const char *name,const int &value)
//------------------------------------------------------------------------------
{
  return StoreText(name,mafString(value));
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreDouble(const char *name,double &value)
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
int mafStorageElement::RestoreInteger(const char *name,int &value)
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
int mafStorageElement::RestoreText(char *&buffer)
//------------------------------------------------------------------------------
{
  mafString tmp;
  if (!RestoreText(tmp))
  {
    buffer=tmp.Duplicate();
    return MAF_OK;
  }

  return MAF_ERROR;
}


//------------------------------------------------------------------------------
int mafStorageElement::RestoreMatrix(const char *name,mafMatrix *matrix)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreMatrix(matrix);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(const char *name,double *comps,unsigned int num)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(const char *name,int *comps,unsigned int num)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(const char *name,std::vector<double> &comps,unsigned int num)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(const char *name,std::vector<int> &comps,unsigned int num)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreVectorN(const char *name,std::vector<mafString> &comps,unsigned int num,const char *tag)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num,tag);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreText(const char *name,char *&buffer)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreText(buffer);
  }
 
  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );
  
  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafStorageElement::RestoreText(const char *name,mafString &buffer)
//------------------------------------------------------------------------------
{
  mafStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreText(buffer);
  }

  mafWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
bool mafStorageElement::GetAttributeAsDouble(const char *name,double &value)
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
bool mafStorageElement::GetAttributeAsInteger(const char *name,mafID &value)
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
void mafStorageElement::SetAttribute(const char *name,const mafID value)
//------------------------------------------------------------------------------
{
  SetAttribute(name,mafString(value));
}
//------------------------------------------------------------------------------
void mafStorageElement::SetAttribute(const char *name,const double value)
//------------------------------------------------------------------------------
{
  SetAttribute(name,mafString(value));
}