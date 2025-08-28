#pragma once

#include "ftk/Base/Object.h"
#include "ftk/Base/String.h"
#include "ftk/IO/Parse.h"

BEGIN_FTK_NAMESPACE

class mafStorageElement;
class mafStorageElementBuilder;

namespace model::data
{
	class FTK_CORE_EXPORT Attribute
	{
	public:
		mafAbstractBaseTypeMacro(Attribute);

		Attribute() = default;

		virtual ~Attribute() = default;

		static std::shared_ptr<Attribute> Create(const char* AttributeType);

		Attribute& operator=(const Attribute& a);

		bool operator==(const Attribute& a) const;

		virtual void DeepCopy(const Attribute* a);

		std::shared_ptr<Attribute> MakeCopy();

		virtual bool Equals(const Attribute* a) const;

		void SetName(const mafString& name);

		const mafString& GetName() const;

		virtual void Print(std::ostream& os, const int tabs = 0) const;

		void Store(mafStorageElementBuilder& element) { InternalStore(element); }

		void Restore(const mafStorageElement& element) { InternalRestore(element); }

	protected:
		virtual void InternalStore(mafStorageElementBuilder& parent);

		virtual void InternalRestore(const mafStorageElement& node);

		mafString m_Name;
	};

	template<class Value>
	std::shared_ptr<Attribute> Parse(const Value& value, io::parse::To<Attribute>)
	{
		mafString type_name = value(_R("Type")).template As<mafString>();
		if (auto attr = Attribute::Create(type_name.GetCStr()))
		{
			attr->Restore(value);
			return attr;
		}
		return nullptr;
	}

	template<class Value>
	void Serialize(Value& value, Attribute* const& attr)
	{
		mafString type_name = _R(attr->GetTypeName());
		assert(attr);
		value(_R("Type")).SetValue(type_name);
		attr->Store(value);
	}
}

using mafAttribute = model::data::Attribute;

END_FTK_NAMESPACE
