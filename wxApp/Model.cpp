#include "Model.h"
#include "Model.h"

namespace ftk
{
	ftkCxxAbstractTypeMacro(Object);
	ftkCxxAbstractTypeMacro(ObjectWithProperties);
	ftkCxxTypeMacro(Node);
	ftkCxxTypeMacro(NodeDerived);

	Object::Object()
	{
	}

	Object::~Object()
	{
	}

	bool Object::operator==(const Object& obj) const
	{
		return Equal(obj);
	}

	bool Object::Equal(const Object& obj) const
	{
		return Equal(&obj);
	}

	Object& Object::DeepCopy(const Object& src)
	{
		return DeepCopy(&src);
	}

	bool Object::Equal(const Object* obj) const
	{
		return GetTypeId() == obj->GetTypeId();
	}

	Object& Object::DeepCopy(const Object* src)
	{
		if (GetTypeId() != src->GetTypeId())
			throw std::bad_cast();
		return *this;
	}
}

