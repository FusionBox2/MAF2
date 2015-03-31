/*=========================================================================

 Program: MAF2
 Module: mafStorageElement
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafStorageElement_h__
#define __mafStorageElement_h__

#include "mafDefines.h"
#include "mafString.h"
#include <vector>
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafParser;
class mafStorable;
class mafMatrix;
class mafObject;


/** Abstract class representing the interface for the unit of information stored in the storage.
  Abstract class representing the interface for the unit of information stored into a storage. A number of utility
  functions are defined to store and restore basic objects into the element. More complex serialization algorithms can
  be implemented by specific "serializable" objects.
  Among the others, the RestoreObject() function is a function which try to restore a mafObject from element, by 
  creating a new instance from the object factory, taking the object name from the "Type" attribute,
  and then calling the Restore function of the newly create object. The newly created object must also be a
  "mafStorable" to support the Restore() function. 
  This abstract class does not implement any real encoding, and subclasses can define specialized de/serialization
  algorithm.  
  @sa mafXMLStorage mafStorageElement mafXMLElement mafStorable
  @todo
  - reimplement children list as a map
 */  
class MAF_EXPORT mafStorageElement
{
public:
  virtual ~mafStorageElement();

  /** get the name of this element. The element name is set at creation time (@sa AppendChild()) */
  const mafString& GetName(){return m_Name;}

  void SetAttribute(const mafString& name,const mafID value);
  void SetAttribute(const mafString& name,const double value);

  bool GetAttributeAsDouble(const mafString& name,double &value);
  bool GetAttributeAsInteger(const mafString& name,mafID &value);

  /** Used to upgrade attribute value from previous MSF file version.*/
  mafString UpgradeAttribute(const mafString& attribute);

  int StoreText   (const mafString& name, const mafString& text);
  int StoreMatrix (const mafString& name,const mafMatrix *matrix);
  int StoreVectorN(const mafString& name, double *comps,int num);
  int StoreVectorN(const mafString& name, int *comps,int num);
  int StoreVectorN(const mafString& name, const std::vector<double> &comps,int num);
  int StoreVectorN(const mafString& name, const std::vector<int> &comps,int num);
  int StoreVectorN(const mafString& name, const std::vector<mafString> &comps,int num,const mafString& tag);

  int RestoreText(const mafString& name,mafString &buffer);
  int RestoreMatrix(const mafString& name,mafMatrix *matrix);
  int RestoreVectorN(const mafString& name,double *comps,unsigned int num);
  int RestoreVectorN(const mafString& name,int *comps,unsigned int num);
  int RestoreVectorN(const mafString& name,std::vector<double> &comps,unsigned int num);
  int RestoreVectorN(const mafString& name,std::vector<int> &comps,unsigned int num);
  int RestoreVectorN(const mafString& name,std::vector<mafString> &comps,unsigned int num,const mafString& tag);

  int RestoreMatrix(mafMatrix *matrix);
  int RestoreVectorN(double *comps,unsigned int num);
  int RestoreVectorN(int *comps,unsigned int num);
  int RestoreVectorN(std::vector<double> &comps,unsigned int num);
  int RestoreVectorN(std::vector<int> &comps,unsigned int num);
  int RestoreVectorN(std::vector<mafString> &comps,unsigned int num,const mafString& tag);

  virtual bool GetAttribute(const mafString& name,mafString &value)=0;
  virtual void SetAttribute(const mafString& name,const mafString& value)=0;
  virtual int StoreText(const mafString &buffer)=0;
  virtual int RestoreText(mafString &buffer)=0;

  int StoreDouble(const mafString& name,const double &value);
  int RestoreDouble(const mafString& name,double &value);
  int RestoreDouble(double &value);

  int StoreInteger(const mafString& name, const int &value);
  int RestoreInteger(const mafString& name,int &value);
  int RestoreInteger(int &value);
  
  /**
    Function to try restoring a mafObject from a mafStorageElement. If the element has
    an attribute with name "Type", the function try to instantiate an object with the same
    name and to restore it from the element. In case of problems the function return NULL. The new
    object is always created by means of New(), this way in case of smart object it can be registered.
    The object to be restored must be both a mafObject (to stay in the object factory) and a mafStorable
    to support Restore() method.*/  
  int RestoreObject(const mafString& name,mafStorable * object);

  /** Restore object from given element. @sa RestoreObject(const char *name,mafObject *&) */
  mafObject *RestoreObject(const mafString& name);

  /** Restore object from given element. @sa RestoreObject(const char *name,mafObject *&) */
  mafObject *RestoreObject();

  /**
    Used for storing a mafObjects. The function simply creates a new element with given 'name' and add an
    attribute with name 'Type' storing the object name. Then the function calls the Store function of the
    object. The object must be a storable object to support Store() interface. The newly stored element is
    returned if OK, NULL in case of problems */
  mafStorageElement *StoreObject(const mafString& name,mafObject *object);

  /** 
    Store a vector of objects. Objects must be both mafObject and mafStorable @sa StoreObject() */
  int StoreObjectVector(const mafString& name,const std::vector<mafObject *> &vector,const mafString& items_name="Item");

  /** Restore a vector of objects. Objects must be both mafObject and mafStorable @sa RestoreObject() */
  int RestoreObjectVector(const mafString& name,std::vector<mafObject *> &vector,const mafString& items_name="Item");

  /** Restore a vector of objects. Objects must be both mafObject and mafStorable @sa RestoreObject() */
  int RestoreObjectVector(mafStorageElement *element,std::vector<mafObject *> &vector,const mafString& items_name="Item");

  /** return a pointer to the storage who created this element */
  mafParser *GetStorage() {return m_Storage;}

  bool GetNestedElementsByName(const mafString& name,std::vector<mafStorageElement *> &list);

  virtual mafStorageElement *AppendChild(const mafString& name) = 0;

  virtual mafStorageElement *FindNestedElement(const mafString& name);

  typedef std::vector<mafStorageElement *> ChildrenVector;

  const ChildrenVector &GetChildren(){return GetChildrenList();}

protected:
  virtual ChildrenVector &GetChildrenList()=0;

  /** elements can be created only by means of AppendChild() or FindNestedElement() */
  mafStorageElement(mafStorageElement *parent,mafParser *storage);

  /** commodity function to store a storable object creating on the fly the element to store it inside. */
  mafStorageElement *StoreObject(const mafString& name,mafStorable *storable, const mafString& type_name);

  void SetStorage(mafParser *storage) {m_Storage = storage;}

  mafParser                        *m_Storage;                        ///< storage who created this element
  std::vector<mafStorageElement *> *m_Children;  ///< children elements
  mafString                        m_Name; ///< Convenient copy of etagName

private:
  int ParseData(std::vector<double> &vector,int size);
  int ParseData(std::vector<int> &vector,int size);
  int ParseData(double *vector,int size);
  int ParseData(int *vector,int size);
};
#endif // _mafStorageElement_h_
