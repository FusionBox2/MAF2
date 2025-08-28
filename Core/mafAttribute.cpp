#include "mafAttribute.h"

#include "mafDefines.h" 

#include "ftk/Core/AttributeFactory.h"
#include "ftk/IO/StorageElement.h"
#include "mafIndent.h"
#include "assert.h"

#include <ostream>
namespace model::data
{
	Attribute& Attribute::operator=(const Attribute& a)
	{
		DeepCopy(&a);
		return *this;
	}

	bool Attribute::operator==(const Attribute& a) const
	{
		return Equals(&a);
	}

	void Attribute::DeepCopy(const Attribute* a)
	{
		assert(a);
		assert(a->IsA(GetTypeId()));
		m_Name = a->GetName();
	}

	std::shared_ptr<Attribute> Attribute::MakeCopy()
	{
		std::shared_ptr<Attribute> new_attr(NewInstance());
		assert(new_attr);
		new_attr->DeepCopy(this);
		return new_attr;
	}

	bool Attribute::Equals(const Attribute* a) const
	{
		return a->IsA(GetTypeId()) && m_Name == a->GetName();
	}

	void Attribute::SetName(const mafString& name)
	{
		m_Name = name;
	}

	const mafString& Attribute::GetName() const
	{
		return m_Name;
	}

	void Attribute::InternalStore(mafStorageElementBuilder& parent)
	{
		parent[_R("Name")].SetValue(m_Name);
	}

	void Attribute::InternalRestore(const mafStorageElement& node)
	{
		m_Name = node[_R("Name")].As<mafString>();
	}

	void Attribute::Print(std::ostream& os, const int tabs) const
	{
		utilities::Indent indent(tabs);

		os << indent << "Attribute Type Name: " << GetTypeName() << "\n";

		os << indent << "Name: " << m_Name.GetCStr() << "\n";
	}

	std::shared_ptr<Attribute> Attribute::Create(const char* AttributeType)
	{
		return AttributeFactory::CreateAttribute(AttributeType);
	}
}