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
#include <map>
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
  /** elements can be created only by means of AppendChild() or FindNestedElement() */
  mafStorageElement(void* element, mafParser* storage);

  virtual ~mafStorageElement();

  /** get the name of this element. The element name is set at creation time (@sa AppendChild()) */
  const mafString& GetName() const {return m_Name;}

  mafStorageElement operator[](const mafString& name) const;
  mafStorageElement operator()(const mafString& name) const;

  void SetAttribute(const mafString& name,const mafID value);
  void SetAttribute(const mafString& name,const double value);

  bool GetAttributeAsDouble(const mafString& name,double &value) const;
  bool GetAttributeAsInteger(const mafString& name,mafID &value) const;

  /** Used to upgrade attribute value from previous MSF file version.*/
  mafString UpgradeAttribute(const mafString& attribute) const;

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

  int RestoreText    (mafString &buffer) const;
  int RestoreInteger (int& value) const;
  int RestoreDouble  (double& value) const;
  int RestoreMatrix  (mafMatrix& matrix) const;
  int RestoreObject  (mafObject*& object) const;
  int RestoreStorable(mafStorable* object) const;
  int RestoreVectorN (double *comps,unsigned int num) const;
  int RestoreVectorN (int *comps,unsigned int num) const;
  int RestoreVectorN (std::vector<double> &comps) const;
  int RestoreVectorN (std::vector<int> &comps) const;
  int RestoreVectorN (std::vector<mafString> &comps,const mafString& tag) const;
  int RestoreVectorN (std::vector<mafObject*>& vector, const mafString& items_name = _R("Item")) const;

  virtual bool GetAttribute(const mafString& name, mafString& value) const;// = 0;
  virtual void SetAttribute(const mafString& name, const mafString& value);// = 0;

protected:
  virtual int StoreText(const mafString& buffer);// = 0;
  int StoreObject(mafObject* object);

public:
  virtual mafStorageElement* AppendChild(const mafString& name);// = 0;
  /** return a pointer to the storage who created this element */
  mafParser *GetStorage()  const {return m_Storage;}

  std::vector<mafStorageElement*> GetElementsByName(const mafString& name) const;


  typedef std::vector<mafStorageElement *> ChildrenVector;

protected:
	virtual void BuildChildrenMap() const;// = 0;


  void SetStorage(mafParser *storage) {m_Storage = storage;}

  mafParser                        *m_Storage;                        ///< storage who created this element
  mutable std::map<mafString, ChildrenVector >*m_Children;  ///< children elements
  mafString                        m_Name; ///< Convenient copy of etagName
  void* m_DOMElement; ///< XML element wrapped by this object (USING PIMPL due to Internal Compile errors of VS7)
};
#endif // _mafStorageElement_h_
