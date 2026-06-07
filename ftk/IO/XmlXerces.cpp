#include "XmlXerces.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <memory>
#include <optional>

namespace
{
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* getDOMNode(void* element)
	{
		if (element)
			return reinterpret_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*>(element);
		return nullptr;
	}

	class XMLPlatformUtilsInitializer
	{
	public:
		XMLPlatformUtilsInitializer()
		{
			// initialize the XML library
			XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
		}
	protected:
		~XMLPlatformUtilsInitializer()
		{
			XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();
		}
	};
	class XercesString
	{
	public:
		XercesString() = default;
		XercesString(const char* str) { m_WStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(str); }
		XercesString(XMLCh* wstr) : m_WStr(wstr) {}
		XercesString(const XMLCh* wstr) { m_WStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(wstr); }
		~XercesString()
		{
			if (m_WStr) XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&m_WStr);
			if (m_CStr) XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&m_CStr);
		}
		const char* GetCharStr() const
		{
			if (m_WStr)
			{
				if (m_CStr == nullptr)
				{
					m_CStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(m_WStr);
				}
				return m_CStr;
			}

			return nullptr;
		}
		operator const XMLCh* () const { return m_WStr; }
		operator const char* () const { return GetCharStr(); }
	protected:
		XMLCh* m_WStr = nullptr;
		mutable char* m_CStr = nullptr;
	};

	class XercesDOMErrorHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ErrorHandler
	{
	public:
		XercesDOMErrorHandler() = default;
		~XercesDOMErrorHandler() override = default;

		void warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch) override{}
		void error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch) override { m_numErrors++; }
		void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch) override { m_numErrors++; }
		void resetErrors() override { m_numErrors = 0; }

		size_t getNumErrors() const { return m_numErrors; }

	private:
		size_t m_numErrors = false;
	};
}

namespace io::xmlxerces
{
	Value::Value() = default;

	Value::Value(const std::vector<void*>& c, std::shared_ptr<Value::NativeHolder> holder)
	{
		m_holder = std::move(holder);
		m_current = c;
		if (m_current.size() == 1)
		{
			auto child_element = getDOMNode(m_current[0])->getFirstChild();
			while (child_element)
			{
				if (child_element->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
				{
					m_children[XercesString(static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(child_element)->getTagName()).GetCharStr()].push_back(child_element);
				}
				child_element = child_element->getNextSibling();
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
		if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
			throw 0;
		if (auto attr = static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(node)->getAttributeNode(XercesString(std::string(key).c_str())))
			return Value({attr}, m_holder);
		return Value({}, m_holder);
	}
	Value Value::operator[](std::string_view key) const
	{
		auto it = m_children.find(std::string(key));
		if (it == m_children.end())
			return Value({}, m_holder);
		return Value(it->second, m_holder);
	}
	Value Value::operator[](std::size_t index) const
	{
		return Value({m_current.at(index)}, m_holder);
	}

	std::size_t Value::size() const
	{
		return m_current.size();
	}

	bool Value::isValid() const
	{
		return m_holder != nullptr && !m_current.empty();
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

namespace io::xmlxerces
{
	//enum PARSER_IO_ERRORS { IO_OK = 0, IO_GENERIC_ERROR, IO_WRONG_OBJECT_TYPE, IO_RESTORE_ERROR, IO_WRONG_FILE_TYPE, IO_WRONG_FILE_VERSION, IO_WRONG_URL, IO_XML_PARSE_ERROR, IO_DOM_XML_ERROR, IO_XML_PARSER_INTERNAL_ERROR, IO_LAST_ERROR };

	class Value::NativeHolder : XMLPlatformUtilsInitializer
	{
	public:
		NativeHolder() = default;

		~NativeHolder() = default;
		std::unique_ptr < XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument> XMLDoc;
	};

	Value Value::FromFile(std::string_view url)
	{
		auto holder = std::make_shared<Value::NativeHolder>();

		auto errReporter = std::make_unique<XercesDOMErrorHandler>();
		auto XMLParser = std::make_unique<XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser>();
		XMLParser->setValidationScheme(XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser::Val_Auto);
		XMLParser->setDoNamespaces(false);
		XMLParser->setDoSchema(false);
		XMLParser->setCreateEntityReferenceNodes(false);

		XMLParser->setErrorHandler(errReporter.get());

		try
		{
			XMLParser->parse(std::string(url).c_str());

			if (XMLParser->getErrorCount() != 0)
			{
				// errors while parsing...
				//mafErrorMessage(_M("Errors while parsing XML file"));
				//errorCode = IO_XML_PARSE_ERROR;
				return {};
			}

			// extract the root element and wrap inside a mafXMLElement
			holder->XMLDoc.reset(XMLParser->adoptDocument());
			auto root = holder->XMLDoc->getDocumentElement();

			Value result;
			result.m_holder = holder;
			result.m_children[XercesString(root->getTagName()).GetCharStr()].push_back(root);
			return result;

			//if (m_impl->fileType != m_impl->m_root->GetName())
			{
				//mafErrorMacro("XML parsing error: wrong file type, expected \"" << m_impl->fileType.GetCStr() << "\", found " << m_impl->m_root->GetName().GetCStr());
				//errorCode = IO_WRONG_FILE_TYPE;
				//return errorCode;
			}
			//auto docVersion = (*m_impl->m_root)(_R("Version")).As<std::optional<mafString> >();
			//if (!docVersion)
			{
				//errorCode = IO_WRONG_FILE_VERSION;
				//return errorCode;
			}
			//double doc_version_f = atof(docVersion->GetCStr());
			//double my_version_f = atof(m_impl->version.GetCStr());

			// 			if (my_version_f > doc_version_f)
			// 			{
			// 				// Paolo 30-11-2007: due to changes on name for mafVMEScalar (to mafVMEScalarMatrix)
			// 				if (doc_version_f < 2.0)
			// 				{
			// 					mafErrorMacro("XML parsing error: wrong file version v" << docVersion->GetCharStr() << ", should be > v" << version.GetCharStr());
			// 					errorCode = IO_WRONG_FILE_VERSION;
			//                     return;
			// 				}
			// 				else
			// 				{
			// 					// Upgrade document to the actual version
			// 					//documentElement->SetAttribute(_R("Version"), my_version_f);
			// 					m_NeedsUpgrade = true;
			// 					if (doc->Restore(documentElement) != MAF_OK)
			// 						errorCode = IO_RESTORE_ERROR;
			// 				}
			// 			}
		}
		catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& e)
		{
			//mafString err;
			//err += _R("An error occurred during XML parsing.\n Message: ");
			//err += _R(mafXMLString(e.getMessage()));
			//mafErrorMessage(_M(err));
			//errorCode = IO_XML_PARSE_ERROR;
		}

		catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& e)
		{
			//mafString err;
			//err += _R("DOM-XML Error while parsing file '") + url + _R("'\n");
			//err += _R("DOMException code is: ") + mafToString(e.code);

			//if (e.getMessage())
			{
				//err += _R("DOMException msg is: ");
				//err += _R(mafXMLString(e.getMessage()));
			}

			//mafErrorMessage(_M(err));
			//errorCode = IO_DOM_XML_ERROR;
		}

		/*catch (const SAXException& e)
		{
			mafString err;
			err << "SAX-XML Error while parsing file: '" << m_ParserURL << "'\n";
			err << "SAXException msg is: " << mafXMLString(e.getMessage());
			mafErrorMessage(err);
		}*/

		catch (...)
		{
			//mafErrorMessage(_M("An error occurred during XML parsing"));
			//errorCode = IO_XML_PARSE_ERROR;
		}
		return {};
	}

	void Value::Store(std::string_view url)
	{
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
		auto node = getDOMNode(value.m_current[0]);
		if (node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE)
		{
			return XercesString(node->getNodeValue()).GetCharStr();
		}
		if (node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		{
			auto child_element = node->getFirstChild();
			while (child_element)
			{
				if (child_element->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TEXT_NODE)
				{
					return XercesString(child_element->getNodeValue()).GetCharStr();
				}
				child_element = child_element->getNextSibling();
			}
			return {};
		};
		return {};
	}
}

namespace io::xmlxerces
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
	/*{
		if (m_value.m_holder == value.m_value.m_holder)
		{
			m_value = value.m_value;
			return *this;
		}
		return *this;
	}*/

	ValueBuilder& ValueBuilder::operator=(ValueBuilder&& value) noexcept = default;
	/*{
		//if (m_value.m_holder == value.m_value.m_holder)
		{
			m_value = std::move(value.m_value);
			return *this;
		}
		auto root = value.m_value["root"];
		auto node = getDOMNode(m_value.m_current[0]);
		auto otherNode = getDOMNode(root.m_current[0]);
		auto child_element = otherNode->getFirstChild();
		while (child_element)
		{
			if (child_element->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TEXT_NODE)
			{
				auto imported = m_value.m_holder->XMLDoc->importNode(child_element, true);
				auto b = static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(node)->getTagName();
				node->appendChild(imported);
				break;
			}
			child_element = child_element->getNextSibling();
		}
		return *this;
	}*/

	ValueBuilder ValueBuilder::operator()(std::string_view key)
	{
		if (!m_value.m_holder)
			throw 0;
		auto node = getDOMNode(m_value.m_current[0]);
		if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
			throw 0;
		if (m_value(key).m_current.empty())
		{
			auto newAttr = node->getOwnerDocument()->createAttribute(XercesString(std::string(key).c_str()));
			//newAttr->setValue(u"");
			static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(node)->setAttributeNode(newAttr);
		}
		return m_value(key);
	}

	ValueBuilder ValueBuilder::operator[](std::string_view key)
	{
		if (!m_value.m_holder)
		{
			m_value.m_holder = std::make_shared<Value::NativeHolder>();
			// get a serializer, an instance of DOMWriter (the "LS" stands for load-save).
			auto XMLImplement = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(XercesString("LS"));
			// create a document
			auto XMLDoc = XMLImplement->createDocument(nullptr, XercesString(std::string(key).c_str()), nullptr); // NO URI and NO DTD
			XMLDoc->setXmlStandalone(true);
			XMLDoc->setXmlVersion(XercesString("1.0"));
			auto root = XMLDoc->getDocumentElement();
			m_value.m_holder->XMLDoc.reset(XMLDoc);
			m_value.m_children[XercesString(root->getTagName()).GetCharStr()].push_back(root);
			return m_value[key];
		}
		if (m_value[key].m_current.empty())
		{
			auto node = getDOMNode(m_value.m_current[0]);
			auto child_element = node->getOwnerDocument()->createElement(XercesString(std::string(key).c_str()));
			node->appendChild(child_element);
			m_value.m_children[std::string(key)].push_back(child_element);
		}
		return m_value[key];
	}

	ValueBuilder ValueBuilder::operator[](std::size_t index)
	{
		if (!m_value.m_holder)
			throw 0;
		auto node = getDOMNode(m_value.m_current[0]);
		if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
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
				auto child = node->getOwnerDocument()->createElement(static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(node)->getTagName());
				node->getParentNode()->appendChild(child);
				m_value.m_current.push_back(child);
			}
			return m_value[index];
		}
	}

	void ValueBuilder::setValue(const std::string& s)
	{
		auto node = getDOMNode(m_value.m_current[0]);
		if (node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE)
		{
			node->setNodeValue(XercesString(s.c_str()));
		}
		if (node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		{
			XERCES_CPP_NAMESPACE_QUALIFIER DOMText* text_node = node->getOwnerDocument()->createTextNode(XercesString(s.c_str()));
			node->appendChild(text_node);
		}
	}

	void ValueBuilder::setValue(const char* s){setValue(std::string(s));}
	void ValueBuilder::setValue(char* s) { setValue(std::string(s)); }
	void ValueBuilder::setValue(std::string_view s){setValue(std::string(s));}

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
