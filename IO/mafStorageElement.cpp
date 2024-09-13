#include "mafStorageElement.h"
#include "ftk/Base/String.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include <vector>
#include <assert.h>
#include <stdio.h>

namespace
{
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* getDOMNode(void* element, size_t numElems, size_t idx = 0)
    {
        if (numElems <= idx)
            throw 0;
        if(numElems == 1)
            return reinterpret_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*>(element);
		return reinterpret_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMNode**>(element)[idx];
    }

	class MAF_EXPORT mafXMLString
	{
	public:
		mafXMLString() : m_WStr(0L), m_CStr(NULL) { };
		mafXMLString(const char* str);
		mafXMLString(XMLCh* wstr);
		mafXMLString(const XMLCh* wstr);
		mafXMLString(const mafXMLString& copy);
		~mafXMLString();
		bool Append(const XMLCh* tail);
		bool Erase(const XMLCh* head, const XMLCh* tail);
		const XMLCh* Begin() const;
		const XMLCh* End() const;
		int Size() const;
		const char* GetCharStr()const;
		XMLCh& operator [] (const int i);
		const XMLCh operator [] (const int i) const;
		operator const XMLCh* () const { return m_WStr; };
		operator const char* () { return GetCharStr(); }
	protected:
		XMLCh* m_WStr;
		mutable char* m_CStr;
	};
	//------------------------------------------------------------------------------
	mafXMLString::mafXMLString(const char* str) : m_WStr(NULL), m_CStr(NULL)
		//------------------------------------------------------------------------------
	{
		m_WStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(str);
	}

	//------------------------------------------------------------------------------
	mafXMLString::mafXMLString(XMLCh* wstr) : m_WStr(wstr), m_CStr(NULL) { };
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	mafXMLString::mafXMLString(const XMLCh* wstr) : m_WStr(NULL), m_CStr(NULL)
		//------------------------------------------------------------------------------
	{
		m_WStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(wstr);
	}

	//------------------------------------------------------------------------------
	mafXMLString::mafXMLString(const mafXMLString& right) : m_WStr(NULL), m_CStr(NULL)
		//------------------------------------------------------------------------------
	{
		m_WStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(right.m_WStr);
	}

	//------------------------------------------------------------------------------
	mafXMLString::~mafXMLString()
		//------------------------------------------------------------------------------
	{
		// thanks tinny!!
		if (m_WStr) XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&m_WStr);
		if (m_CStr) XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&m_CStr);
	}

	//------------------------------------------------------------------------------
	bool mafXMLString::Append(const XMLCh* tail)
		//------------------------------------------------------------------------------
	{
		int iTailLen = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(tail);
		int iWorkLen = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(m_WStr);
		XMLCh* result = new XMLCh[iWorkLen + iTailLen + 1];
		bool bOK = result != NULL;
		if (bOK)
		{
			XMLCh* target = result;
			XERCES_CPP_NAMESPACE_QUALIFIER XMLString::moveChars(target, m_WStr, iWorkLen);
			target += iWorkLen;
			XERCES_CPP_NAMESPACE_QUALIFIER XMLString::moveChars(target, tail, iTailLen);
			target += iTailLen;
			*target++ = 0;
			XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&m_WStr);
			m_WStr = result;
		}
		return bOK;
	}

	//------------------------------------------------------------------------------
	bool mafXMLString::Erase(const XMLCh* head, const XMLCh* tail)
		//------------------------------------------------------------------------------
	{
		bool bOK = head <= tail && head >= Begin() && tail <= End();
		if (bOK)
		{
			XMLCh* result = new XMLCh[Size() - (tail - head) + 1];
			XMLCh* target = result;
			bOK = target != NULL;
			if (bOK)
			{
				const XMLCh* cursor = Begin();

				while (cursor != head) *target++ = *cursor++;
				cursor = tail;
				while (cursor != End()) *target++ = *cursor++;
				*target++ = 0;
				XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&m_WStr);
				m_WStr = result;
			}
		}
		return bOK;
	}

	//------------------------------------------------------------------------------
	const XMLCh* mafXMLString::Begin() const
		//------------------------------------------------------------------------------
	{
		return m_WStr;
	}

	//------------------------------------------------------------------------------
	const XMLCh* mafXMLString::End() const
		//------------------------------------------------------------------------------
	{
		return m_WStr + Size();
	}

	//------------------------------------------------------------------------------
	int mafXMLString::Size() const
		//------------------------------------------------------------------------------
	{
		return XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(m_WStr);
	}

	//------------------------------------------------------------------------------
	XMLCh& mafXMLString::operator [] (const int i)
		//------------------------------------------------------------------------------
	{
		return m_WStr[i];
	}

	//------------------------------------------------------------------------------
	const XMLCh mafXMLString::operator [] (const int i) const
		//------------------------------------------------------------------------------
	{
		return m_WStr[i];
	}

	//------------------------------------------------------------------------------
	const char* mafXMLString::GetCharStr()const
		//------------------------------------------------------------------------------
	{
		if (m_WStr)
		{
			if (m_CStr == NULL)
			{
				m_CStr = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(m_WStr);
			}

			return m_CStr;
		}

		return NULL;
	}
}

namespace parser
{
	mafString Parse(const mafStorageElement& value, To<mafString>)
	{
		auto node = getDOMNode(value.GetImpl(), value.GetNumItems());
		if (node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE)
		{
			return _R(mafXMLString(node->getNodeValue()));
		}
		if (node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		{
			auto child_element = node->getFirstChild();
			while (child_element)
			{
				if (child_element->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::TEXT_NODE)
				{
					return _R(mafXMLString(child_element->getNodeValue()));
				}
				child_element = child_element->getNextSibling();
			}
			return {};
		};
		return {};
	}

	int Parse(const mafStorageElement& value, To<int>)
    {
		return std::stoi(value.As<mafString>().toStd());
    }
	mafID Parse(const mafStorageElement& value, To<mafID>)
	{
		return std::stoll(value.As<mafString>().toStd());
	}
	double Parse(const mafStorageElement& value, To<double>)
	{
		return std::stod(value.As<mafString>().toStd());
	}
}

namespace serializer
{
	void Serialize(mafStorageElementBuilder& value, const mafString& val)
	{
		auto node = getDOMNode(value.GetImpl(), value.GetNumItems());
		if (node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE)
		{
			node->setNodeValue(mafXMLString(val.GetCStr()));
		}
		if (node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		{
			XERCES_CPP_NAMESPACE_QUALIFIER DOMText* text_node = node->getOwnerDocument()->createTextNode(mafXMLString(val.GetCStr()));
			node->appendChild(text_node);
		}
	}
 	void Serialize(mafStorageElementBuilder& value, const int& val)
	{
		value.SetValue(mafToString(val));
	}
	void Serialize(mafStorageElementBuilder& value, const int64_t& val)
	{
		value.SetValue(mafToString(val));
	}
	void Serialize(mafStorageElementBuilder& value, const uint64_t& val)
	{
		value.SetValue(mafToString((long long)val));
	}
	void Serialize(mafStorageElementBuilder& value, const double& val)
	{
		value.SetValue(mafToString(val));
	}
}

/*namespace serializer::json
{
	void Serialize(mafStorageElementBuilder& value, const mafString& val)
	{
		auto node = getJson(value.GetImpl());
		(*node) = val.toStd();
	}
	void Serialize(mafStorageElementBuilder& value, const int& val)
	{
		auto node = getJson(value.GetImpl());
		(*node) = val;
	}
	void Serialize(mafStorageElementBuilder& value, const int64_t& val)
	{
		auto node = getJson(value.GetImpl());
		(*node) = val;
	}
	void Serialize(mafStorageElementBuilder& value, const uint64_t& val)
	{
		auto node = getJson(value.GetImpl());
		(*node) = val;
	}
	void Serialize(mafStorageElementBuilder& value, const double& val)
	{
		auto node = getJson(value.GetImpl());
		(*node) = val;
	}
}*/

//------------------------------------------------------------------------------
mafStorageElement::mafStorageElement(void* element, mafXMLReader* storage)
//------------------------------------------------------------------------------
{
	m_Storage = storage;
	m_DOMElement = element;
	if (element == nullptr)
	{
		m_NumItems = 0;
		return;
	}
	m_NumItems = 1;
	auto child_element = getDOMNode(m_DOMElement, m_NumItems)->getFirstChild();
	while (child_element)
	{
		if (child_element->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		{
			mafString name(_R(mafXMLString(static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(child_element)->getTagName())));
			m_Children[name].push_back(child_element);
		}
		child_element = child_element->getNextSibling();
	}
}

//------------------------------------------------------------------------------
mafStorageElement::mafStorageElement(void* const* elements, size_t numElems, mafXMLReader* storage)
//------------------------------------------------------------------------------
{
	m_Storage = storage;
	m_NumItems = numElems;
	if (numElems == 1)
	{
		m_DOMElement = elements[0];
		auto child_element = getDOMNode(m_DOMElement, m_NumItems)->getFirstChild();
		while (child_element)
		{
			if (child_element->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
			{
				mafString name(_R(mafXMLString(static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(child_element)->getTagName())));
				m_Children[name].push_back(child_element);
			}
			child_element = child_element->getNextSibling();
		}
	}
	else
	{
		auto src = reinterpret_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* const*>(elements);
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode** buf = new XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * [numElems];
		std::copy(src, src + numElems, buf);
		m_DOMElement = buf;
	}
}

//------------------------------------------------------------------------------
mafStorageElement::~mafStorageElement()
//------------------------------------------------------------------------------
{
	if (m_NumItems > 1)
		delete[] reinterpret_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMNode**>(m_DOMElement);
}

//------------------------------------------------------------------------------
mafString mafStorageElement::GetName() const
//------------------------------------------------------------------------------
{
  auto node = getDOMNode(m_DOMElement, m_NumItems);
	if (node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return _R(mafXMLString(static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(node)->getTagName()));
	return _R("");
}

//------------------------------------------------------------------------------
mafStorageElement mafStorageElement::operator[](const mafString& name) const
//------------------------------------------------------------------------------
{
	auto it = m_Children.find(name);
	if (it == m_Children.end())
		return mafStorageElement(nullptr, GetStorage());
	return mafStorageElement(it->second.data(), it->second.size(), GetStorage());
}

//------------------------------------------------------------------------------
mafStorageElement mafStorageElement::operator()(const mafString& name) const
//------------------------------------------------------------------------------
{
  auto node = getDOMNode(m_DOMElement, m_NumItems);
	if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		throw 0;
	return mafStorageElement(static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(node)->getAttributeNode(mafXMLString(name.GetCStr())), GetStorage());
}

//------------------------------------------------------------------------------
mafStorageElement mafStorageElement::operator[](size_t idx) const
//------------------------------------------------------------------------------
{
	return mafStorageElement(getDOMNode(m_DOMElement, m_NumItems, idx), GetStorage());
}

//------------------------------------------------------------------------------
size_t mafStorageElement::GetNumItems() const
//------------------------------------------------------------------------------
{
	return m_NumItems;
}

//------------------------------------------------------------------------------
bool mafStorageElement::IsValid() const
//------------------------------------------------------------------------------
{
	return m_DOMElement != nullptr;
}

void *mafStorageElement::GetImpl() const
{
    return m_DOMElement;
}

#ifndef JSON_Builder
//------------------------------------------------------------------------------
mafStorageElementBuilder::mafStorageElementBuilder(void* element)
//------------------------------------------------------------------------------
{
	m_DOMElement = element;
  m_NumItems = 1;
}
//------------------------------------------------------------------------------
mafStorageElementBuilder::~mafStorageElementBuilder()
//------------------------------------------------------------------------------
{
	if (m_NumItems > 1)
		delete[] reinterpret_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMNode**>(m_DOMElement);
}

mafStorageElementBuilder mafStorageElementBuilder::operator[](const mafString& name)
{
  auto node = getDOMNode(m_DOMElement, m_NumItems);
  auto child_element = node->getOwnerDocument()->createElement(mafXMLString(name.GetCStr()));
  node->appendChild(child_element);
  return mafStorageElementBuilder(child_element);
}

mafStorageElementBuilder mafStorageElementBuilder::operator()(const mafString& name)
{
  auto node = getDOMNode(m_DOMElement, m_NumItems);
  if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
    throw 0;
  auto newAttr = node->getOwnerDocument()->createAttribute(mafXMLString(name.GetCStr()));
  static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(node)->setAttributeNode(newAttr);
  return mafStorageElementBuilder(newAttr);
}

mafStorageElementBuilder mafStorageElementBuilder::operator[](size_t idx)
{
	auto node = getDOMNode(m_DOMElement, m_NumItems);
	if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		throw 0;
	if (idx == npos)
		idx = m_NumItems;
	if(idx < m_NumItems)
	{
		return mafStorageElementBuilder(getDOMNode(m_DOMElement, m_NumItems, idx));
	}
	else
	{
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode** src = nullptr;
		if (m_NumItems == 1)
		{
			src = reinterpret_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMNode**>(&m_DOMElement);
		}
		else
		{
			src = reinterpret_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMNode**>(m_DOMElement);
		}

		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode** buf = new XERCES_CPP_NAMESPACE_QUALIFIER DOMNode*[idx + 1];
		std::copy(src, src + m_NumItems, buf);
		if (m_NumItems > 1)
			delete[] reinterpret_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMNode**>(m_DOMElement);
		m_DOMElement = buf;
		for (size_t i = m_NumItems; i < idx + 1; i++)
		{
			buf[i] = node->getOwnerDocument()->createElement(static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>(node)->getTagName());
			node->getParentNode()->appendChild(buf[i]);
		}
		m_NumItems = idx + 1;
		return mafStorageElementBuilder(getDOMNode(m_DOMElement, m_NumItems, idx));
	}
}

//------------------------------------------------------------------------------
size_t mafStorageElementBuilder::GetNumItems() const
//------------------------------------------------------------------------------
{
	return m_NumItems;
}

//------------------------------------------------------------------------------
bool mafStorageElementBuilder::IsValid() const
//------------------------------------------------------------------------------
{
	return m_DOMElement != nullptr;
}

void* mafStorageElementBuilder::GetImpl() const
{
	return m_DOMElement;
}

//------------------------------------------------------------------------------
mafString mafStorageElement::UpgradeAttribute(const mafString& attribute) const
//------------------------------------------------------------------------------
{
  mafString att_name;
  /*mafString new_att_name;
  GetAttribute(attribute, att_name);
  if (att_name == _R("mafVMEItemScalar"))
  {
    new_att_name = _R("mafVMEItemScalarMatrix");
    SetAttribute(attribute, new_att_name);
    return new_att_name;
  }
  if (att_name.find(_R("mafVME")) != mafString::npos)
  {
    if (att_name == _R("mafVMEScalar"))
    {
      new_att_name = _R("mafVMEScalarMatrix");
      SetAttribute(attribute, new_att_name);
    }
    else
    {
      new_att_name = att_name;
    }
    mafStorageElement data_vector = FindNestedElement(_R("DataVector"));
    mafString item_type;
    if (data_vector && data_vector->GetAttribute(_R("ItemTypeName"), item_type))
    {
      if (item_type == _R("mafVMEItemScalar"))
      {
        data_vector->SetAttribute(_R("ItemTypeName"), _R("mafVMEItemScalarMatrix"));
      }
    }
    return new_att_name;
  }*/
  
  return att_name;
}

#else
#include "libjson/json.h"
namespace
{
	Json::Value* getJson(void* element)
	{
		return reinterpret_cast<Json::Value*>(element);
	}
}

//------------------------------------------------------------------------------
mafStorageElementBuilder::mafStorageElementBuilder(void* element)
//------------------------------------------------------------------------------
{
	m_DOMElement = element;
}
//------------------------------------------------------------------------------
mafStorageElementBuilder::~mafStorageElementBuilder()
//------------------------------------------------------------------------------
{
}

mafStorageElementBuilder mafStorageElementBuilder::operator[](const mafString& name)
{
	auto child_element = getJson(m_DOMElement);
	return mafStorageElementBuilder(&(*child_element)[name.toStd()]);
}

mafStorageElementBuilder mafStorageElementBuilder::operator()(const mafString& name)
{
	auto child_element = getJson(m_DOMElement);
	return mafStorageElementBuilder(&(*child_element)["attr"][name.toStd()]);
}

mafStorageElementBuilder mafStorageElementBuilder::operator[](size_t idx)
{
	auto child_element = getJson(m_DOMElement);
	if(idx == npos)
		idx = GetNumItems();
	return mafStorageElementBuilder(&(*child_element)[(int)idx]);
}

//------------------------------------------------------------------------------
size_t mafStorageElementBuilder::GetNumItems() const
//------------------------------------------------------------------------------
{
	auto child_element = getJson(m_DOMElement);
	return child_element->size();
}

//------------------------------------------------------------------------------
bool mafStorageElementBuilder::IsValid() const
//------------------------------------------------------------------------------
{
	return m_DOMElement != nullptr;
}

void* mafStorageElementBuilder::GetImpl() const
{
	return m_DOMElement;
}

//------------------------------------------------------------------------------
mafString mafStorageElement::UpgradeAttribute(const mafString& attribute) const
//------------------------------------------------------------------------------
{
	mafString att_name;
	/*mafString new_att_name;
	GetAttribute(attribute, att_name);
	if (att_name == _R("mafVMEItemScalar"))
	{
	  new_att_name = _R("mafVMEItemScalarMatrix");
	  SetAttribute(attribute, new_att_name);
	  return new_att_name;
	}
	if (att_name.find(_R("mafVME")) != mafString::npos)
	{
	  if (att_name == _R("mafVMEScalar"))
	  {
		new_att_name = _R("mafVMEScalarMatrix");
		SetAttribute(attribute, new_att_name);
	  }
	  else
	  {
		new_att_name = att_name;
	  }
	  mafStorageElement data_vector = FindNestedElement(_R("DataVector"));
	  mafString item_type;
	  if (data_vector && data_vector->GetAttribute(_R("ItemTypeName"), item_type))
	  {
		if (item_type == _R("mafVMEItemScalar"))
		{
		  data_vector->SetAttribute(_R("ItemTypeName"), _R("mafVMEItemScalarMatrix"));
		}
	  }
	  return new_att_name;
	}*/

	return att_name;
}

//------------------------------------------------------------------------------
int mafStorageElementBuilder::Store_Text(const mafString& text)
//------------------------------------------------------------------------------
{
	auto child_element = getJson(m_DOMElement);
	(*child_element) = text.toStd();
	return MAF_OK;
}

#endif


#include <xercesc/util/XercesDefs.hpp>

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>


namespace
{
	class XMLPlatformUtilsInitializer
	{
	public:
		XMLPlatformUtilsInitializer()
		{
			// initialize the XML library
			XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
		}
		~XMLPlatformUtilsInitializer()
		{
			XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();
		}
	};

	class mmuDOMTreeErrorReporter : public XERCES_CPP_NAMESPACE_QUALIFIER ErrorHandler
	{
	public:
		/** constructor */
		mmuDOMTreeErrorReporter() : m_SawErrors(false), m_TestFlag(false) {}
		/** destructor */
		~mmuDOMTreeErrorReporter() override {}

		/** Implementation of the warning handler interface */
		void warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch) override;
		/** Implementation of the error handler interface */
		void error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch) override;
		/** Implementation of the fatal error handler interface */
		void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch) override;
		/** reset error flag */
		void resetErrors() override;

		/** retrieve error flag */
		bool GetSawErrors() const { return m_SawErrors; }
		/** set test modality in order to skip, problematic log messages*/
		void SetTestMode(bool enable) { m_TestFlag = enable; }

	private:
		bool    m_SawErrors; ///< Set if we get any errors, used by the main code to suppress output if there are errors.
		bool m_TestFlag;
	};
	//------------------------------------------------------------------------------
	void mmuDOMTreeErrorReporter::warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException&)
		//------------------------------------------------------------------------------
	{
		// Ignore all warnings.
	}
	//------------------------------------------------------------------------------
	void mmuDOMTreeErrorReporter::error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch)
		//------------------------------------------------------------------------------
	{
		m_SawErrors = true;
		if (m_TestFlag == false)
		{
			mafErrorMessageMacro("Error at file \"" << toCatch.getSystemId() \
				<< "\", line " << toCatch.getLineNumber() \
				<< ", column " << toCatch.getColumnNumber() \
				<< "\n   Message: " << mafXMLString(toCatch.getMessage()) \
			);
		}
	}
	//------------------------------------------------------------------------------
	void mmuDOMTreeErrorReporter::fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& toCatch)
		//------------------------------------------------------------------------------
	{
		m_SawErrors = true;
		if (m_TestFlag == false)
		{
			mafErrorMessageMacro("Fatal Error at file \"" << mafXMLString(toCatch.getSystemId()) \
				<< "\", line " << toCatch.getLineNumber() \
				<< ", column " << toCatch.getColumnNumber() \
				<< "\n   Message: " << mafXMLString(toCatch.getMessage()) \
			);
		}

	}
	//------------------------------------------------------------------------------
	void mmuDOMTreeErrorReporter::resetErrors()
		//------------------------------------------------------------------------------
	{
		m_SawErrors = false;
	}
}

class mafXMLReaderImpl : XMLPlatformUtilsInitializer
{
public:
	mafXMLReaderImpl() {}
	std::unique_ptr<mmuDOMTreeErrorReporter> errReporter;
	std::unique_ptr<XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser> XMLParser;
	std::optional<mafStorageElement> m_root;
	mafString fileType;
	mafString version;
	mafString URL;
};

mafXMLReader::mafXMLReader(const mafString& fileType, const mafString& version)
{
	m_impl = std::make_unique<mafXMLReaderImpl>();
	m_impl->fileType = fileType;
	m_impl->version = version;
	//
	//  Create our parser, then attach an error handler to the parser.
	//  The parser will call back to methods of the ErrorHandler if it
	//  discovers errors during the course of parsing the XML document.
	//
	m_impl->errReporter = std::make_unique<mmuDOMTreeErrorReporter>();
	m_impl->XMLParser = std::make_unique<XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser>();
	m_impl->XMLParser->setValidationScheme(XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser::Val_Auto);
	m_impl->XMLParser->setDoNamespaces(false);
	m_impl->XMLParser->setDoNamespaces(false);
	m_impl->XMLParser->setDoSchema(false);
	m_impl->XMLParser->setCreateEntityReferenceNodes(false);

	m_impl->XMLParser->setErrorHandler(m_impl->errReporter.get());
}

mafXMLReader::~mafXMLReader()
{
}

int mafXMLReader::Load(const mafString& url)
{
	int errorCode = MAF_OK;
	m_impl->URL = url;
	try
	{
		m_impl->XMLParser->parse(url.GetCStr());
		int errorCount = m_impl->XMLParser->getErrorCount();

		if (errorCount != 0)
		{
			// errors while parsing...
			mafErrorMessage(_M("Errors while parsing XML file"));
			errorCode = IO_XML_PARSE_ERROR;
			return errorCode;
		}
		// extract the root element and wrap inside a mafXMLElement
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* XMLDoc = m_impl->XMLParser->getDocument();
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* root = XMLDoc->getDocumentElement();
		m_impl->m_root = mafStorageElement(root, this);

		if (m_impl->fileType != m_impl->m_root->GetName())
		{
			mafErrorMacro("XML parsing error: wrong file type, expected \"" << m_impl->fileType.GetCStr() << "\", found " << m_impl->m_root->GetName().GetCStr());
			errorCode = IO_WRONG_FILE_TYPE;
			return errorCode;
		}
		auto docVersion = (*m_impl->m_root)(_R("Version")).As<std::optional<mafString> >();
		if (!docVersion)
		{
			errorCode = IO_WRONG_FILE_VERSION;
			return errorCode;
		}
		double doc_version_f = atof(docVersion->GetCStr());
		double my_version_f = atof(m_impl->version.GetCStr());

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
		mafString err;
		err += _R("An error occurred during XML parsing.\n Message: ");
		err += _R(mafXMLString(e.getMessage()));
		mafErrorMessage(_M(err));
		errorCode = IO_XML_PARSE_ERROR;
	}

	catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& e)
	{
		mafString err;
		err += _R("DOM-XML Error while parsing file '") + url + _R("'\n");
		err += _R("DOMException code is: ") + mafToString(e.code);

		if (e.getMessage())
		{
			err += _R("DOMException msg is: ");
			err += _R(mafXMLString(e.getMessage()));
		}

		mafErrorMessage(_M(err));
		errorCode = IO_DOM_XML_ERROR;
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
		mafErrorMessage(_M("An error occurred during XML parsing"));
		errorCode = IO_XML_PARSE_ERROR;
	}
	return errorCode;
}

const mafStorageElement& mafXMLReader::GetRoot() const
{
	return *m_impl->m_root;
}

const mafString& mafXMLReader::GetURL() const
{
	return m_impl->URL;
}

#ifndef JSON_Builder
class mafXMLWriterImpl : XMLPlatformUtilsInitializer
{
public:
	mafXMLWriterImpl() {}

	XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation* XMLImplement = nullptr;
	std::unique_ptr<XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument> XMLDoc;
	std::optional<mafStorageElementBuilder> m_root;
};

mafXMLWriter::mafXMLWriter(const mafString& fileType, const mafString& version)
{
	m_impl = std::make_unique<mafXMLWriterImpl>();
	// get a serializer, an instance of DOMWriter (the "LS" stands for load-save).
	m_impl->XMLImplement = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(mafXMLString("LS"));
	// create a document
	m_impl->XMLDoc.reset(m_impl->XMLImplement->createDocument(NULL, mafXMLString(fileType.GetCStr()), NULL)); // NO URI and NO DTD
	m_impl->XMLDoc->setXmlStandalone(true);
	m_impl->XMLDoc->setXmlVersion(mafXMLString("1.0"));
	m_impl->m_root = mafStorageElementBuilder(m_impl->XMLDoc->getDocumentElement());
	// attach version attribute to the root node
	(*m_impl->m_root)(_R("Version")).SetValue(version);
}
mafXMLWriter::~mafXMLWriter()
{
}

mafStorageElementBuilder& mafXMLWriter::GetRoot()
{
	return *m_impl->m_root;
}

int mafXMLWriter::Save(const mafString& url)
{
	int errorCode = MAF_OK;

	std::unique_ptr<XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget> XMLTarget = std::make_unique<XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget>(url.GetCStr());
	std::unique_ptr<XERCES_CPP_NAMESPACE_QUALIFIER DOMLSSerializer> XMLSerializer(((XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS*)m_impl->XMLImplement)->createLSSerializer());
	// set user specified end of line sequence and output encoding
	XMLSerializer->setNewLine(mafXMLString("\r"));

	// set serializer features 
	XERCES_CPP_NAMESPACE_QUALIFIER DOMConfiguration* config = XMLSerializer->getDomConfig();
	config->setParameter(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTSplitCdataSections, false);
	config->setParameter(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTDiscardDefaultContent, false);
	config->setParameter(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);
	config->setParameter(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTBOM, false);

	try
	{
		XERCES_CPP_NAMESPACE_QUALIFIER DOMLSOutput* theOutputDesc = m_impl->XMLImplement->createLSOutput();
		// output related nodes are prefixed with "svg"
		// to distinguish them from input nodes.
		theOutputDesc->setEncoding(mafXMLString("UTF-8"));
		theOutputDesc->setByteStream(XMLTarget.get());
		XMLSerializer->write(m_impl->XMLDoc.get(), theOutputDesc);
		// destroy all intermediate objects
		theOutputDesc->release();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& e)
	{
		mafErrorMessageMacro("XML error, DOMException code is:  " << e.code);
		errorCode = 2;
	}
	catch (...)
	{
		mafErrorMessage(_M("XML error, an error occurred creating the XML document!"));
		errorCode = 3;
	}
	return errorCode;
}
#else
#include "json.h"
#include <fstream>
class mafXMLWriterImpl : XMLPlatformUtilsInitializer
{
public:
	mafXMLWriterImpl() {}
	std::optional<mafStorageElementBuilder> m_root;
	Json::Value m_rootValue;
};

mafXMLWriter::mafXMLWriter(const mafString& fileType, const mafString& version)
{
	m_impl = std::make_unique<mafXMLWriterImpl>();
	m_impl->m_root = mafStorageElementBuilder(&m_impl->m_rootValue);
	// attach version attribute to the root node
	(*m_impl->m_root)(_R("Version")).SetValue(version);
}
mafXMLWriter::~mafXMLWriter()
{
}

mafStorageElementBuilder& mafXMLWriter::GetRoot()
{
	return *m_impl->m_root;
}

int mafXMLWriter::Save(const mafString& url)
{
	int errorCode = MAF_OK;
	std::ofstream ofs(url.toStd());
	Json::FastWriter writer;
	ofs << writer.write(m_impl->m_rootValue);
	return errorCode;
}
#endif
