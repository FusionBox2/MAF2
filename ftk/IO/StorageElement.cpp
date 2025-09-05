#include "ftk/IO/StorageElement.h"

#include "ftk/IO/XmlXerces.h"
#include "ftk/IO/JsonRapid.h"

#include <optional>

BEGIN_FTK_NAMESPACE

class mafStorageElement::StorageElemImpl
{
public:
	io::xmlxerces::Value m_value;
	io::Reader *m_storage = nullptr;
};

mafStorageElement::mafStorageElement() = default;
mafStorageElement::mafStorageElement(const mafStorageElement&) = default;
mafStorageElement::mafStorageElement(mafStorageElement&&) noexcept = default;
mafStorageElement& mafStorageElement::operator=(const mafStorageElement&) = default;
mafStorageElement& mafStorageElement::operator=(mafStorageElement&&) noexcept = default;
mafStorageElement::~mafStorageElement() = default;

mafStorageElement mafStorageElement::operator[](std::string_view name) const
{
	mafStorageElement result;
	result.m_impl->m_value = m_impl->m_value[name];
	result.m_impl->m_storage = m_impl->m_storage;
	return result;
}

mafStorageElement mafStorageElement::operator()(std::string_view name) const
{
	mafStorageElement result;
	result.m_impl->m_value = m_impl->m_value(name);
	result.m_impl->m_storage = m_impl->m_storage;
	return result;
}

mafStorageElement mafStorageElement::operator[](const mafString& name) const
{
	return (*this)[name.toStd()];
}

mafStorageElement mafStorageElement::operator()(const mafString& name) const
{
	return (*this)(name.toStd());
}

mafStorageElement mafStorageElement::operator[](size_t idx) const
{
	mafStorageElement result;
	result.m_impl->m_value = m_impl->m_value[idx];
	result.m_impl->m_storage = m_impl->m_storage;
	return result;
}

bool mafStorageElement::isValid() const
{
	return m_impl->m_value.isValid();
}

size_t mafStorageElement::size() const
{
	return m_impl->m_value.size();
}

io::Reader *mafStorageElement::GetStorage() const
{
	return m_impl->m_storage;
}

namespace io::parse
{
	std::string Parse(const mafStorageElement& value, io::parse::To<std::string>)
	{
		return value.m_impl->m_value.As<std::string>();
	}

	mafString Parse(const mafStorageElement& value, io::parse::To<mafString>)
	{
		return _R(value.m_impl->m_value.As<std::string>().c_str());
	}

	intmax_t Parse(const mafStorageElement& value, io::parse::To<intmax_t>)
	{
		return value.m_impl->m_value.As<intmax_t>();
	}

	uintmax_t Parse(const mafStorageElement& value, io::parse::To<uintmax_t>)
	{
		return value.m_impl->m_value.As<uintmax_t>();
	}

	double Parse(const mafStorageElement& value, io::parse::To<double>)
	{
		return value.m_impl->m_value.As<double>();
	}
}

class mafStorageElementBuilder::StorageElemBuildImpl
{
public:
	io::xmlxerces::ValueBuilder m_builder;
};

mafStorageElementBuilder::mafStorageElementBuilder() = default;
mafStorageElementBuilder::mafStorageElementBuilder(const mafStorageElementBuilder&) = default;
mafStorageElementBuilder::mafStorageElementBuilder(mafStorageElementBuilder&&) noexcept = default;
mafStorageElementBuilder& mafStorageElementBuilder::operator=(const mafStorageElementBuilder&) = default;
mafStorageElementBuilder& mafStorageElementBuilder::operator=(mafStorageElementBuilder&&) noexcept = default;
mafStorageElementBuilder::~mafStorageElementBuilder() = default;

mafStorageElementBuilder mafStorageElementBuilder::operator[](std::string_view name)
{
	mafStorageElementBuilder result;
	result.m_impl->m_builder = m_impl->m_builder[name];
	return result;
}

mafStorageElementBuilder mafStorageElementBuilder::operator()(std::string_view name)
{
	mafStorageElementBuilder result;
	result.m_impl->m_builder = m_impl->m_builder(name);
	return result;
}

mafStorageElementBuilder mafStorageElementBuilder::operator[](const mafString& name)
{
	return (*this)[name.toStd()];
}

mafStorageElementBuilder mafStorageElementBuilder::operator()(const mafString& name)
{
	return (*this)(name.toStd());
}

mafStorageElementBuilder mafStorageElementBuilder::operator[](size_t idx)
{
	mafStorageElementBuilder result;
	result.m_impl->m_builder = m_impl->m_builder[idx];
	return result;
}

size_t mafStorageElementBuilder::size() const
{
	return m_impl->m_builder.size();
}

bool mafStorageElementBuilder::isValid() const
{
	return m_impl->m_builder.isValid();
}

namespace io
{
	class ReaderImpl
	{
	public:
		ReaderImpl() = default;
		std::optional<mafStorageElement> m_root;
		mafString fileType;
		mafString version;
		mafString URL;
	};

	Reader::Reader(const mafString& fileType, const mafString& version)
	{
		m_impl = std::make_unique<ReaderImpl>();
		m_impl->fileType = fileType;
		m_impl->version = version;
	}

	Reader::~Reader() = default;

	int Reader::Load(const mafString& url)
	{
		m_impl->URL = url;
		m_impl->m_root = mafStorageElement();
		m_impl->m_root->m_impl->m_value = io::xmlxerces::Value::FromFile(m_impl->URL.toStd());
		m_impl->m_root->m_impl->m_storage = this;
		return IO_OK;
	}

	mafStorageElement Reader::GetRoot() const
	{
		return (*m_impl->m_root)[m_impl->fileType];
	}

	const mafString& Reader::GetURL() const
	{
		return m_impl->URL;
	}

	class WriterImpl
	{
	public:
		mafStorageElementBuilder m_root;
		mafString fileType;
	};

	Writer::Writer(const mafString& fileType, const mafString& version)
	{
		m_impl = std::make_unique<WriterImpl>();
		m_impl->fileType = fileType;
		m_impl->m_root[m_impl->fileType](_R("Version")).SetValue(version);
	}

	Writer::~Writer() = default;

	mafStorageElementBuilder Writer::GetRoot()
	{
		return m_impl->m_root[m_impl->fileType];
	}

	int Writer::Save(const mafString& url) const
	{
		m_impl->m_root.m_impl->m_builder.extractValue().Store(url.toStd());
		return IO_OK;
	}
}

namespace serializer
{
	void Serialize(mafStorageElementBuilder& value, const std::string& val)
	{
		value.m_impl->m_builder.setValue(val);
	}

	void Serialize(mafStorageElementBuilder& value, const mafString& val)
	{
		value.m_impl->m_builder.setValue(val.toStd());
	}

	void Serialize(mafStorageElementBuilder& value, const intmax_t& val)
	{
		value.m_impl->m_builder.setValue(val);
	}

	void Serialize(mafStorageElementBuilder& value, const uintmax_t& val)
	{
		value.m_impl->m_builder.setValue(val);
	}

	void Serialize(mafStorageElementBuilder& value, const double& val)
	{
		value.m_impl->m_builder.setValue(val);
	}
}

END_FTK_NAMESPACE
