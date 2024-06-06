/*=========================================================================

 Program: MAF2
 Module: mafAttribute
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafAttribute_h
#define __mafAttribute_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafReferenceCounted.h" 
#include "mafObjectFactory.h"
#include "mafString.h"
#include "mafTo.h"

class mafStorageElement;
class mafStorageElementBuilder;

/** An abstract class for objects representing an attribute for mafNodes.
  This abstract class represent the interface of an attribute for mafNodes. An attribute
  is a bunch of data that can be attached to a node. Attributes can be specialized classes
  providing any kind of information and functionality. The minimal required features are
  RTTI, object factory registration and Storability. Any attribute can be serialized and 
  then recreated from the factory and unserialized.
  @sa mafNode mafStorable mafObject
*/
class MAF_EXPORT mafAttribute : public mafReferenceCounted
{
public:
  mafAbstractTypeMacro(mafAttribute,mafReferenceCounted);

  /** attributes must define a copy rule */
  void operator=(const mafAttribute &a);

  bool operator==(const mafAttribute &a) const;

  /** copy the content of the given attribute. Attributes must be type compatible */
  virtual void DeepCopy(const mafAttribute *a);

  /** create a copy of this attribute */
  mafAttribute *MakeCopy();

  /** return true if this attribute equals the given one */
  bool Equals(const mafAttribute *a) const;

  /** 
    Set the name of this attribute. this is typically used to identify it
    inside the Attributes associative map stored inside a mafNode */
  void SetName(const mafString& name);
  const mafString& GetName() const;

  /** dump the object to output stream */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  void Store(mafStorageElementBuilder& element) { InternalStore(element); }
  void Restore(const mafStorageElement& element) { InternalRestore(element); }

protected:
  virtual void InternalStore(mafStorageElementBuilder& parent);
  virtual void InternalRestore(const mafStorageElement& node);

  mafString m_Name;
};

namespace parser
{
  template<class Value>
  mafAttribute* Parse(const Value& value, parser::To<mafAttribute>)
  {
    mafString type_name = value(_R("Type")).As<mafString>();
    auto object = mafObjectFactory::CreateInstance(type_name.GetCStr());
    if (auto attr = mafAttribute::SafeDownCast(object))
    {
      attr->Restore(value);
      return attr;
    }
    return nullptr;
  }
}

namespace serializer
{
  template<class Value>
  void Serialize(Value& value, mafAttribute* const& attr)
  {
    mafString type_name = _R(attr->GetTypeName());
    assert(attr);
    value(_R("Type")).SetValue(type_name);
    attr->Store(value);
  }
}

#endif

