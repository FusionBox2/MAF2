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

  int StoreText    (const mafString& name, const mafString& text);
  int StoreInteger (const mafString& name, const int& value);
  int StoreDouble  (const mafString& name, const double& value);
  int StoreMatrix  (const mafString& name, const mafMatrix& matrix);
  int StoreObject  (const mafString& name, mafObject* object);
  int StoreStorable(const mafString& name, mafStorable* object);
  int StoreVectorN (const mafString& name, double *comps,int num);
  int StoreVectorN (const mafString& name, int *comps,int num);
  int StoreVectorN (const mafString& name, const std::vector<double> &comps);
  int StoreVectorN (const mafString& name, const std::vector<int> &comps);
  int StoreVectorN (const mafString& name, const std::vector<mafString> &comps,const mafString& tag);
  int StoreVectorN (const mafString& name, const std::vector<mafObject*>& vector, const mafString& items_name = _R("Item"));

  int RestoreText    (const mafString& name, mafString &buffer);
  int RestoreInteger (const mafString& name, int& value);
  int RestoreDouble  (const mafString& name, double& value);
  int RestoreMatrix  (const mafString& name, mafMatrix& matrix);
  int RestoreObject  (const mafString& name, mafObject*& object);
  int RestoreStorable(const mafString& name, mafStorable* object);
  int RestoreVectorN (const mafString& name, double *comps,unsigned int num);
  int RestoreVectorN (const mafString& name, int *comps,unsigned int num);
  int RestoreVectorN (const mafString& name, std::vector<double> &comps);
  int RestoreVectorN (const mafString& name, std::vector<int> &comps);
  int RestoreVectorN (const mafString& name, std::vector<mafString> &comps,const mafString& tag);
  int RestoreVectorN (const mafString& name, std::vector<mafObject*>& vector, const mafString& items_name = _R("Item"));

  int RestoreMatrix(mafMatrix& matrix);
  int RestoreVectorN(double *comps,unsigned int num);
  int RestoreVectorN(std::vector<mafString> &comps,const mafString& tag);

  virtual bool GetAttribute(const mafString& name,mafString &value)=0;
  virtual void SetAttribute(const mafString& name,const mafString& value)=0;

  int RestoreObject(mafObject*& object);
protected:
  int RestoreVectorN(int *comps,unsigned int num);
  virtual int StoreText(const mafString &buffer)=0;
  virtual int RestoreText(mafString& buffer) = 0;
  int StoreObject(mafObject* object);

public:
  virtual mafStorageElement* FindNestedElement(const mafString& name);
  virtual mafStorageElement* AppendChild(const mafString& name) = 0;
  /** return a pointer to the storage who created this element */
  mafParser *GetStorage() {return m_Storage;}

  bool GetNestedElementsByName(const mafString& name,std::vector<mafStorageElement *> &list);


  typedef std::vector<mafStorageElement *> ChildrenVector;

  const ChildrenVector &GetChildren(){return GetChildrenList();}

protected:
  virtual ChildrenVector &GetChildrenList()=0;

  /** elements can be created only by means of AppendChild() or FindNestedElement() */
  mafStorageElement(mafParser *storage);


  void SetStorage(mafParser *storage) {m_Storage = storage;}

  mafParser                        *m_Storage;                        ///< storage who created this element
  std::vector<mafStorageElement *> *m_Children;  ///< children elements
  mafString                        m_Name; ///< Convenient copy of etagName
};
#endif // _mafStorageElement_h_
