#pragma once

#include "ftk/Base/Object.h"
#include "ftk/Base/String.h"
#include "mafReferenceCounted.h" 
#include "mafTo.h"

class mafStorageElement;
class mafStorageElementBuilder;

class FTK_CORE_EXPORT mafAttribute : public mafReferenceCounted
{
public:
  mafAbstractTypeMacro(mafAttribute,mafReferenceCounted);

  static mafAttribute* Create(const char* AttributeType);

  mafAttribute& operator=(const mafAttribute &a);

  bool operator==(const mafAttribute &a) const;

  virtual void DeepCopy(const mafAttribute *a);

  mafAttribute *MakeCopy();

  bool Equals(const mafAttribute *a) const;

  void SetName(const mafString& name);

	const mafString& GetName() const;

  void Print(std::ostream& os, const int tabs=0) const override;

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
    mafString type_name = value(_R("Type")).template As<mafString>();
    if (auto attr = mafAttribute::Create(type_name.GetCStr()))
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
