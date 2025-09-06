#include "XmlRapid.h"

#include "impl/rapidxml.hpp"
#include "impl/rapidxml_print.hpp"
#include "impl/rapidxml_utils.hpp"

namespace
{
	rapidxml::xml_node<>* getDOMNode(void* element)
	{
		if (element)
			return reinterpret_cast<rapidxml::xml_node<>*>(element);
		return nullptr;
	}

	rapidxml::xml_attribute<>* getDOMAttr(void* element)
	{
		if (element)
			return reinterpret_cast<rapidxml::xml_attribute<>*>(element);
		return nullptr;
	}
}

namespace io::xmlrapid
{
	Value::Value() = default;

	Value::Value(const std::vector<void*>& c, void* attr, std::shared_ptr<Value::NativeHolder> holder)
	{
		m_holder = std::move(holder);
		m_current = c;
		m_attr = attr;
		if (m_current.size() == 1)
		{
			auto child_element = getDOMNode(m_current[0])->first_node();
			while (child_element)
			{
				if (child_element->type() == rapidxml::node_element)
				{
					m_children[child_element->name()].push_back(child_element);
				}
				child_element = child_element->next_sibling();
			}
		}
	}

	Value::Value(const Value& value) = default;
	Value::Value(Value&& value) noexcept = default;

	Value::~Value() = default;

	Value& Value::operator=(const Value& value) = default;
	Value& Value::operator=(Value&& value) noexcept = default;

	Value Value::operator()(std::string_view key) const
	{
		auto node = getDOMNode(m_current.front());
		if (node->type() != rapidxml::node_element)
			throw 0;
		if (auto attr = node->first_attribute(key.data(), key.size()))
			return Value({}, attr, m_holder);
		return Value({}, nullptr, m_holder);
	}
	Value Value::operator[](std::string_view key) const
	{
		auto it = m_children.find(std::string(key));
		if (it == m_children.end())
			return Value({}, nullptr, m_holder);
		return Value(it->second, nullptr, m_holder);
	}
	Value Value::operator[](std::size_t index) const
	{
		return Value({ m_current.at(index) }, nullptr, m_holder);
	}

	std::size_t Value::size() const
	{
		return m_current.size();
	}

	bool Value::isValid() const
	{
		return m_holder != nullptr;
	}

	bool Value::isMissing() const
	{
		return m_holder != nullptr;
	}

	bool Value::isNull() const
	{
		return m_holder != nullptr;
	}
}

namespace io::xmlrapid
{
	//enum PARSER_IO_ERRORS { IO_OK = 0, IO_GENERIC_ERROR, IO_WRONG_OBJECT_TYPE, IO_RESTORE_ERROR, IO_WRONG_FILE_TYPE, IO_WRONG_FILE_VERSION, IO_WRONG_URL, IO_XML_PARSE_ERROR, IO_DOM_XML_ERROR, IO_XML_PARSER_INTERNAL_ERROR, IO_LAST_ERROR };

	class Value::NativeHolder
	{
	public:
		Value::NativeHolder() = default;

		~NativeHolder() = default;
		std::unique_ptr<rapidxml::xml_document<>> XMLDoc;
		std::vector<char> buffer;
	};

	Value Value::FromFile(std::string_view url)
	{
		rapidxml::file<> xmlFile(std::string(url).c_str());
		auto m_impl = std::make_shared<Value::NativeHolder>();
		m_impl->buffer.assign(xmlFile.data(), xmlFile.data() + xmlFile.size());
		m_impl->XMLDoc = std::make_unique<rapidxml::xml_document<>>();
		m_impl->XMLDoc->parse<0>(m_impl->buffer.data());
		Value result;
		auto rootNode = m_impl->XMLDoc->first_node();
		result.m_children[rootNode->name()].push_back(rootNode);
		result.m_holder = m_impl;
		return result;
	}

	void Value::Store(std::string_view url)
	{
		std::ofstream file{ std::string(url) };
		file << *m_holder->XMLDoc;
#ifdef jkhkhk
		int errorCode = 0;
		auto m_impl = m_holder;

		auto XMLImplement = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(XercesString("LS"));
		// create a document
		//m_impl->XMLDoc.reset(XMLImplement->createDocument(NULL, XercesString("MAF"), NULL)); // NO URI and NO DTD
		//m_impl->XMLDoc->setXmlStandalone(true);
		//m_impl->XMLDoc->setXmlVersion(XercesString("1.0"));
		//m_impl->m_root = mafStorageElementBuilder(m_impl->XMLDoc->getDocumentElement());
		// attach version attribute to the root node
		//(*m_impl->m_root)(_R("Version")).SetValue(version);


		std::unique_ptr<XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget> XMLTarget = std::make_unique<XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget>((const XMLCh*)XercesString(std::string(url).c_str()));
		std::unique_ptr<XERCES_CPP_NAMESPACE_QUALIFIER DOMLSSerializer> XMLSerializer(((XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS*)XMLImplement)->createLSSerializer());
		// set user specified end of line sequence and output encoding
		XMLSerializer->setNewLine(XercesString("\r"));

		// set serializer features 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMConfiguration* config = XMLSerializer->getDomConfig();
		config->setParameter(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTSplitCdataSections, false);
		config->setParameter(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTDiscardDefaultContent, false);
		config->setParameter(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);
		config->setParameter(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTBOM, false);

		try
		{
			XERCES_CPP_NAMESPACE_QUALIFIER DOMLSOutput* theOutputDesc = XMLImplement->createLSOutput();
			// output related nodes are prefixed with "svg"
			// to distinguish them from input nodes.
			theOutputDesc->setEncoding(XercesString("UTF-8"));
			theOutputDesc->setByteStream(XMLTarget.get());
			XMLSerializer->write(m_impl->XMLDoc.get(), theOutputDesc);
			// destroy all intermediate objects
			theOutputDesc->release();
		}
		catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& e)
		{
			//mafErrorMessageMacro("XML error, DOMException code is:  " << e.code);
			errorCode = 2;
		}
		catch (...)
		{
			//mafErrorMessage(_M("XML error, an error occurred creating the XML document!"));
			errorCode = 3;
		}
		//return errorCode;
#endif
	}

	intmax_t Parse(const Value& value, io::parse::To<intmax_t>)
	{
		return std::stoll(value.As<std::string>());
	}

	uintmax_t Parse(const Value& value, io::parse::To<uintmax_t>)
	{
		return std::stoll(value.As<std::string>());
	}

	double Parse(const Value& value, io::parse::To<double>)
	{
		return std::stod(value.As<std::string>());
	}

	std::string Parse(const Value& value, io::parse::To<std::string>)
	{
		if (value.m_attr)
		{
			return getDOMAttr(value.m_attr)->value();
		}
		auto node = getDOMNode(value.m_current[0]);
		return node->value();
	}
}

namespace io::xmlrapid
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

	ValueBuilder& ValueBuilder::operator=(const ValueBuilder& value)
	{
		if (m_value.m_holder == value.m_value.m_holder)
		{
			m_value = value.m_value;
			return *this;
		}
		return *this;
	}

	ValueBuilder& ValueBuilder::operator=(ValueBuilder&& value) noexcept
	{
		if (m_value.m_holder == value.m_value.m_holder)
		{
			m_value = std::move(value.m_value);
			return *this;
		}
		auto root = value.m_value["root"];
		auto node = getDOMNode(m_value.m_current[0]);
		auto otherNode = getDOMNode(root.m_current[0]);
		//auto child_element = otherNode->getFirstChild();
		//while (child_element)
		{
			//if (child_element->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TEXT_NODE)
			{
				//auto imported = m_value.m_holder->XMLDoc->importNode(child_element, true);
				//auto b = static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(node)->getTagName();
				//node->appendChild(imported);
				//break;
			}
			//child_element = child_element->getNextSibling();
		}
		return *this;
	}

	ValueBuilder ValueBuilder::operator()(std::string_view key)
	{
		if (!m_value.m_holder)
			throw 0;
		auto node = getDOMNode(m_value.m_current[0]);
		if (node->type() != rapidxml::node_element)
			throw 0;
		if (m_value(key).m_current.empty())
		{
			auto attr_name = m_value.m_holder->XMLDoc->allocate_string(std::string(key).c_str());
			auto newAttr = node->document()->allocate_attribute(attr_name);
			//newAttr->setValue(u"");
			node->append_attribute(newAttr);
		}
		return m_value(key);
	}

	ValueBuilder ValueBuilder::operator[](std::string_view key)
	{
		if (!m_value.m_holder)
		{
			m_value.m_holder = std::make_shared<Value::NativeHolder>();
			m_value.m_holder->XMLDoc = std::make_unique<rapidxml::xml_document<>>();
			auto decl = m_value.m_holder->XMLDoc->allocate_node(rapidxml::node_element);
			decl->append_attribute(m_value.m_holder->XMLDoc->allocate_attribute("version", "1.0"));
			decl->append_attribute(m_value.m_holder->XMLDoc->allocate_attribute("encoding", "utf-8"));
			m_value.m_holder->XMLDoc->append_node(decl);
			auto node_name = m_value.m_holder->XMLDoc->allocate_string(std::string(key).c_str());
			auto root = m_value.m_holder->XMLDoc->allocate_node(rapidxml::node_element, node_name);
			m_value.m_holder->XMLDoc->append_node(root);
			m_value.m_children[std::string(key)].push_back(root);
			return m_value[key];
		}
		if (m_value[key].m_current.empty())
		{
			auto node = getDOMNode(m_value.m_current[0]);
			auto node_name = m_value.m_holder->XMLDoc->allocate_string(std::string(key).c_str());
			auto child_element = node->document()->allocate_node(rapidxml::node_element, node_name);
			node->append_node(child_element);
			m_value.m_children[std::string(key)].push_back(child_element);
		}
		return m_value[key];
	}

	ValueBuilder ValueBuilder::operator[](std::size_t index)
	{
		if (!m_value.m_holder)
			throw 0;
		auto node = getDOMNode(m_value.m_current[0]);
		if (node->type() != rapidxml::node_element)
			throw 0;
		if (index == static_cast<size_t>(-1))
			index = m_value.m_current.size();
		if (index < m_value.m_current.size())
		{
			return m_value[index];
		}
		else
		{
			for (size_t i = m_value.m_current.size(); i < index + 1; i++)
			{
				auto child = node->document()->allocate_node(rapidxml::node_element, node->name());
				node->parent()->append_node(child);
				m_value.m_current.push_back(child);
			}
			return m_value[index];
		}
	}

	void ValueBuilder::setValue(const std::string& s)
	{
		auto s_value = m_value.m_holder->XMLDoc->allocate_string(s.c_str());
		if (m_value.m_attr)
		{
			getDOMAttr(m_value.m_attr)->value(s_value);
		}
		else
		{
			getDOMNode(m_value.m_current[0])->value(s_value);
		}
	}

	void ValueBuilder::setValue(const char* s) { setValue(std::string(s)); }
	void ValueBuilder::setValue(char* s) { setValue(std::string(s)); }
	void ValueBuilder::setValue(std::string_view s) { setValue(std::string(s)); }

	std::size_t ValueBuilder::size() const
	{
		return m_value.size();
	}

	bool ValueBuilder::isValid() const
	{
		return m_value.isValid();
	}

	Value ValueBuilder::extractValue()
	{
		return m_value;
	}

	void Serialize(ValueBuilder& t, intmax_t i, io::serialize::To<ValueBuilder>)
	{
		t.setValue(std::to_string(i));
	}
	void Serialize(ValueBuilder& t, uintmax_t u, io::serialize::To<ValueBuilder>)
	{
		t.setValue(std::to_string(u));
	}
	void Serialize(ValueBuilder& t, double d, io::serialize::To<ValueBuilder>)
	{
		t.setValue(std::to_string(d));
	}
}

END_FTK_NAMESPACE
