#include "JsonRapid.h"

#include <fstream>
#include <libjson/json.h>

namespace
{
	Json::Value* getJson(void* element)
	{
		if (element)
			return reinterpret_cast<Json::Value*>(element);
		return nullptr;
	}
}

BEGIN_FTK_NAMESPACE

namespace io::jsonrapid
{
	Value::Value() = default;

	Value::Value(void* p, std::shared_ptr<NativeHolder> holder) : m_holder(std::move(holder)), m_current(p) {}

	Value::Value(const Value& value) = default;
	Value::Value(Value&& value) noexcept = default;

	Value::~Value() = default;

	Value& Value::operator=(const Value& value) = default;
	Value& Value::operator=(Value&& value) noexcept = default;

	Value Value::operator()(std::string_view key) const
	{
		const auto& node = *getJson(m_current);
		return Value(const_cast<void*>(reinterpret_cast<const void*>(&node["_attr"][std::string(key)])), m_holder);
	}
	Value Value::operator[](std::string_view key) const
	{
		const auto& node = *getJson(m_current);
		return Value(const_cast<void*>(reinterpret_cast<const void*>(&node[std::string(key)])), m_holder);
	}
	Value Value::operator[](std::size_t index) const
	{
		const auto& node = *getJson(m_current);
		return Value(const_cast<void*>(reinterpret_cast<const void*>(&node[(int)index])), m_holder);
	}

	std::size_t Value::size() const
	{
		return 1;
	}

	bool Value::isValid() const
	{
		return false;
	}
}

namespace io::jsonrapid
{
	class Value::NativeHolder
	{
	public:
		Value::NativeHolder() = default;

		~NativeHolder() = default;
		Json::Value m_value;
	};

	Value Value::FromFile(std::string_view url)
	{
		std::ifstream ifs;
		ifs.open(std::string(url));
		auto m_impl = std::make_shared<Value::NativeHolder>();
		ifs >> m_impl->m_value;
		return Value(&m_impl->m_value, m_impl);
	}

	void Value::Store(std::string_view url)
	{
		std::ofstream ofs;
		ofs.open(std::string(url));
		ofs << *getJson(m_current);
	}


	intmax_t Parse(const Value&, io::parse::To<intmax_t>)
	{
		return 7;
	}

	uintmax_t Parse(const Value&, io::parse::To<uintmax_t>)
	{
		return 9;
	}

	bool Parse(const Value&, io::parse::To<bool>)
	{
		return true;
	}

	double Parse(const Value&, io::parse::To<double>)
	{
		return 3.0;
	}

	std::string Parse(const Value& value, io::parse::To<std::string>)
	{
		const auto& node = *getJson(value.m_current);
		return node.asString();
	}

}

namespace io::jsonrapid
{
	ValueBuilder::ValueBuilder() = default;
	ValueBuilder::ValueBuilder(const ValueBuilder& value) = default;
	ValueBuilder::ValueBuilder(ValueBuilder&& value) noexcept = default;

	ValueBuilder::ValueBuilder(const Value& other)
		: m_value(other)
	{
	}

	ValueBuilder::ValueBuilder(Value&& other) noexcept
		: m_value(std::move(other))
	{
	}

	ValueBuilder& ValueBuilder::operator=(const ValueBuilder& value) = default;
	ValueBuilder& ValueBuilder::operator=(ValueBuilder&& value) noexcept = default;

	ValueBuilder ValueBuilder::operator()(std::string_view key)
	{
		auto& node = *getJson(m_value.m_current);
		return Value(reinterpret_cast<void*>(&node["_attr"][std::string(key)]), m_value.m_holder);
	}
	ValueBuilder ValueBuilder::operator[](std::string_view key)
	{
		if (!m_value.m_holder)
		{
			m_value.m_holder = std::make_shared<Value::NativeHolder>();
			m_value.m_current = &m_value.m_holder->m_value;
		}
		auto& node = *getJson(m_value.m_current);
		return Value(reinterpret_cast<void*>(&node[std::string(key)]), m_value.m_holder);
	}
	ValueBuilder ValueBuilder::operator[](std::size_t index)
	{
		auto& node = *getJson(m_value.m_current);
		return Value(reinterpret_cast<void*>(&node[(int)index]), m_value.m_holder);
	}

	std::size_t ValueBuilder::size() const
	{
		return 1;
	}

	bool ValueBuilder::isValid() const
	{
		return false;
	}

	void ValueBuilder::setValue(const std::string& s)
	{
		auto& node = *getJson(m_value.m_current);
		node = s;
	}

	void ValueBuilder::setValue(const char* s) { setValue(std::string(s)); }
	void ValueBuilder::setValue(char* s) { setValue(std::string(s)); }
	void ValueBuilder::setValue(std::string_view s) { setValue(std::string(s)); }

	Value ValueBuilder::extractValue()
	{
		return m_value;
	}

	void Serialize(ValueBuilder& b, int i, io::serialize::To<ValueBuilder>)
	{
		b.setValue(std::to_string(i));
	}
}

END_FTK_NAMESPACE
