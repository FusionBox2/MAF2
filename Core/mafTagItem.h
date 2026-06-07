#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/String.h"
#include "ftk/IO/Parse.h"

#include <vector>
#include <iosfwd>

BEGIN_FTK_NAMESPACE

namespace model::data
{
	class FTK_CORE_EXPORT TagItem final
	{
	public:
		TagItem();
		~TagItem();

		enum TAG_IDS { MAF_MISSING_TAG = 0, MAF_NUMERIC_TAG, MAF_STRING_TAG };
		/** Constructors for with implicit Tag type...*/
		TagItem(const mafString& name, const mafString& value, int t = MAF_STRING_TAG);
		TagItem(const mafString& name, const mafString* values, int numcomp, int t = MAF_STRING_TAG);
		TagItem(const mafString& name, const std::vector<mafString>& values, int t = MAF_STRING_TAG);
		TagItem(const mafString& name, double value);
		TagItem(const mafString& name, const double* value, int numcomp);
		TagItem(const mafString& name, const std::vector<double>& values);

		TagItem(const TagItem& p);
		TagItem& operator=(const TagItem& p);

		bool operator==(const TagItem& p) const;
		bool operator!=(const TagItem& p) const;

		bool Equals(const TagItem* item) const;

		void DeepCopy(const TagItem* item);

		const mafString& GetName() const;

		void SetName(const mafString& name);

		void SetValue(const mafString& value, int component = 0);

		void SetComponent(const mafString& value, int component = 0);

		void SetValue(double value, int component = 0);

		void SetComponent(double value, int component = 0);

		void SetComponents(const mafString* values, int numcomp);

		void SetComponents(const std::vector<mafString>& components);

		void SetValues(const std::vector<mafString>& values);

		void SetValues(const mafString* values, int numcomp);

		void RemoveValue(int component);

		void SetValues(const double* values, int numcomp);

		void SetValues(const std::vector<double>& values);

		void SetComponents(const double* components, int numcomp);

		void SetComponents(const std::vector<double>& components);

		const mafString& GetValue(int component = 0) const;

		const mafString& GetComponent(int comp) const;

		double GetValueAsDouble(int component = 0) const;

		double GetComponentAsDouble(int comp) const;

		const std::vector<mafString>& GetComponents() const { return m_Components; }

		mafString GetValueAsSingleString() const;

		int GetType() const { return m_Type; }

		void SetType(int t) { m_Type = t; }

		mafString GetTypeAsString() const;

		int GetNumberOfComponents() const;

		void SetNumberOfComponents(int n);

		void Print(std::ostream& os, int tabs = 0) const;

	protected:

		void Initialize();

		mafString m_Name;
		int m_Type;
		std::vector<mafString> m_Components;
	};

	template<class Value>
	TagItem Parse(const Value& value, io::parse::To<TagItem>)
	{
		mafString type = value(_R("Type")).template As<mafString>();

		int typeValue = 0;
		if (type == _R("NUM"))
		{
			typeValue = TagItem::MAF_NUMERIC_TAG;
		}
		else if (type == _R("STR"))
		{
			typeValue = TagItem::MAF_STRING_TAG;
		}
		else if (type == _R("MIS"))
		{
			typeValue = TagItem::MAF_MISSING_TAG;
		}
		else
		{
		}
		mafID num = value(_R("Mult")).template As<mafID>();
		return TagItem(value(_R("Name")).template As<mafString>(), value[_R("TItem")][_R("TC")].template As<std::vector<mafString>>(), typeValue);
	}

	template<class Value>
	void Serialize(Value& value, const TagItem& item)
	{
		value(_R("Name")).SetValue(item.GetName());
		value(_R("Mult")).SetValue(item.GetNumberOfComponents());
		value(_R("Type")).SetValue(item.GetTypeAsString());
		auto TItem = value[_R("TItem")];
		for (auto& comp : item.GetComponents())
		{
			TItem[_R("TC")][Value::npos].SetValue(comp);
		}
	}

}

using mafTagItem = model::data::TagItem;

END_FTK_NAMESPACE