#include "mafTagItem.h"

#include "mafIndent.h"

#include <assert.h>

namespace model::data
{
	TagItem::TagItem()
	{
		Initialize();
	}

	TagItem::~TagItem() = default;

	TagItem::TagItem(const mafString& name, const mafString& value, int t)
	{
		Initialize();
		SetValue(value);
		SetName(name);
		SetType(t);
	}

	TagItem::TagItem(const mafString& name, const mafString* values, int numcomp, int t)
	{
		Initialize();
		SetName(name);
		SetValues(values, numcomp);
		SetType(t);
	}

	TagItem::TagItem(const mafString& name, const std::vector<mafString>& values, int t)
	{
		Initialize();
		SetName(name);
		SetValues(values);
		SetType(t);
	}

	TagItem::TagItem(const mafString& name, double value)
	{
		Initialize();
		SetName(name);
		SetValue(value);
	}

	TagItem::TagItem(const mafString& name, const double* value, int numcomp)
	{
		Initialize();
		SetName(name);
		SetValues(value, numcomp);

	}

	TagItem::TagItem(const mafString& name, const std::vector<double>& values)
	{
		Initialize();
		SetName(name);
		SetValues(values);
		SetType(MAF_NUMERIC_TAG);
	}

	void TagItem::DeepCopy(const TagItem* item)
	{
		assert(item);
		SetName(item->GetName());
		SetComponents(item->GetComponents());
		SetType(item->GetType());
	}

	TagItem& TagItem::operator=(const TagItem& p)
	{
		DeepCopy(&p);
		return *this;
	}

	TagItem::TagItem(const TagItem& p)
	{
		Initialize();
		*this = p;
	}

	bool TagItem::operator==(const TagItem& p) const
	{
		return Equals(&p);
	}

	bool TagItem::operator!=(const TagItem& p) const
	{
		return !Equals(&p);
	}

	void TagItem::SetName(const mafString& name)
	{
		m_Name = name;
	}

	const mafString& TagItem::GetName() const
	{
		return m_Name;
	}

	void TagItem::SetValue(double value, int component)
	{
		mafString tmp = mafToString(value);
		SetValue(tmp, component);
		SetType(MAF_NUMERIC_TAG);
	}

	void TagItem::SetComponent(const mafString& value, int component)
	{
		SetValue(value, component);
	}

	void TagItem::SetComponent(double value, int component)
	{
		SetValue(value, component);
	}

	void TagItem::SetValue(const mafString& value, int component)
	{
		if (component >= GetNumberOfComponents())
			SetNumberOfComponents(component + 1);
		m_Components[component] = value;
		SetType(MAF_STRING_TAG);
	}

	void TagItem::SetValues(const mafString* values, int numcomp)
	{
		// if the number of component differs, reallocate memory
		if (numcomp != GetNumberOfComponents())
		{
			this->SetNumberOfComponents(numcomp);
		}

		// copy data
		for (int i = 0; i < numcomp; i++)
		{
			this->SetValue(values[i], i);
		}
	}

	void TagItem::SetValues(const std::vector<mafString>& values)
	{
		m_Components = values;
	}

	void TagItem::SetValues(const double* values, int numcomp)
	{
		// if the number of component differs, reallocate memory
		if (numcomp != GetNumberOfComponents())
		{
			this->SetNumberOfComponents(numcomp);
		}

		// copy data
		for (int i = 0; i < numcomp; i++)
		{
			this->SetValue(values[i], i);
		}
	}

	void TagItem::SetValues(const std::vector<double>& values)
	{
		// if the number of component differs, reallocate memory
		if (values.size() != GetNumberOfComponents())
		{
			this->SetNumberOfComponents(values.size());
		}

		// copy data
		for (int i = 0; i < values.size(); i++)
		{
			this->SetValue(values[i], i);
		}
	}

	void TagItem::SetComponents(const mafString* values, int numcomp)
	{
		SetValues(values, numcomp);
	}

	void TagItem::SetComponents(const std::vector<mafString>& components)
	{
		SetValues(components);
	}

	void TagItem::SetComponents(const double* components, int numcomp)
	{
		SetValues(components, numcomp);
	}

	void TagItem::SetComponents(const std::vector<double>& components)
	{
		SetValues(components);
	}

	const mafString& TagItem::GetValue(int component) const
	{
		static mafString empty;
		if (GetNumberOfComponents() > component)
			return m_Components[component];
		return empty;
	}

	void TagItem::RemoveValue(int component)
	{
		if (GetNumberOfComponents() > component)
			m_Components.erase(m_Components.begin() + component);
	}

	const mafString& TagItem::GetComponent(int comp) const
	{
		return GetValue(comp);
	}

	double TagItem::GetValueAsDouble(int component) const
	{
		if (GetNumberOfComponents() <= component)
			return 0;
		return atof(GetValue(component).GetCStr());
	}

	double TagItem::GetComponentAsDouble(int comp) const
	{
		return GetValueAsDouble(comp);
	}

	void TagItem::Initialize()
	{
		m_Name = _R("");
		m_Type = MAF_MISSING_TAG;
		m_Components.clear();
	}

	int TagItem::GetNumberOfComponents() const
	{
		return m_Components.size();
	}

	void TagItem::SetNumberOfComponents(int n)
	{
		m_Components.resize(n);
	}

	mafString TagItem::GetValueAsSingleString() const
	{
		if (GetNumberOfComponents() > 0)
		{
			mafString str = _R("(");

			for (int i = 0; i < GetNumberOfComponents(); i++)
			{
				// colon separator
				if (i > 0) str += _R(",");

				str += _R("\"") + m_Components[i] + _R("\"");
			}
			str += _R(")");
			return str;
		}
		return _R("");
	}

	mafString TagItem::GetTypeAsString() const
	{
		switch (m_Type)
		{
		case (MAF_MISSING_TAG):
			return _R("MIS");
		case (MAF_NUMERIC_TAG):
			return _R("NUM");
		case (MAF_STRING_TAG):
			return _R("STR");
		}
		return _R("UNK");
	}

	bool TagItem::Equals(const TagItem* item) const
	{
		if (!item)
			return false;
		return m_Name == item->m_Name && m_Type == item->m_Type && m_Components == item->m_Components;
	}

	void TagItem::Print(std::ostream& os, int tabs) const
	{
		mafIndent indent(tabs);

		os << indent << "Name: \"" << (m_Name.GetCStr() ? m_Name.GetCStr() : _R("(NULL)")) << "\"";

		int t = this->GetType();
		const char* tstr;

		switch (t)
		{
		case (MAF_MISSING_TAG):
			tstr = "MISSING";
			break;
		case (MAF_NUMERIC_TAG):
			tstr = "NUMERIC";
			break;
		case (MAF_STRING_TAG):
			tstr = "STRING";
			break;
		default:
			tstr = "UNKNOWN";
		}


		os << " Type: " << tstr << " (" << t << ") ";

		os << " Components: ";

		if (GetNumberOfComponents() > 0)
		{
			os << GetValueAsSingleString().GetCStr();
			os << " NumComp: " << GetNumberOfComponents();
		}
		os << std::endl; // end of single line printing
	}
}