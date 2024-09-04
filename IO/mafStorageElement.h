#ifndef __mafStorageElement_h__
#define __mafStorageElement_h__

#include "mafDefines.h"
#include "mafString.h"
#include "mafTo.h"

#include <vector>
#include <map>
#include <optional>

class mafXMLReader;

class MAF_EXPORT mafStorageElement
{
public:
  mafStorageElement(void* element, mafXMLReader* storage);
  mafStorageElement(void* const * elements, size_t numElems, mafXMLReader* storage);
  ~mafStorageElement();

  mafString GetName() const;

  mafStorageElement operator[](const mafString& name) const;
  mafStorageElement operator()(const mafString& name) const;
  mafStorageElement operator[](size_t idx) const;

  template<typename T>
  auto As() const;

  mafXMLReader* GetStorage()  const {return m_Storage;}

  mafString UpgradeAttribute(const mafString& attribute) const;

  bool      IsValid()     const;
  void*     GetImpl()     const;
  size_t    GetNumItems() const;

protected:
  void*                                     m_DOMElement;
  size_t                                    m_NumItems;
  mafXMLReader*                             m_Storage;
  std::map<mafString, std::vector<void*> >  m_Children;
};

template<typename T>
auto mafStorageElement::As() const
{
  return Parse(*this, parser::To<T>{});
}

namespace parser
{
  mafString Parse(const mafStorageElement& value, To<mafString>);
  int Parse(const mafStorageElement& value, To<int>);
  mafID Parse(const mafStorageElement& value, To<mafID>);
  double Parse(const mafStorageElement& value, To<double>);
  template <typename T, typename Value>
  std::optional<decltype(Parse(std::declval<Value>(), To<T>{})) >
  Parse(const Value& value, To<std::optional<T>>)
  {
    if (!value.IsValid())
    {
      return std::nullopt;
    }
    return value.template As<T>();
  }
  template <typename T, typename Value>
  auto Parse(const Value& value, To<std::vector<T> >)
  {
    std::vector<decltype(Parse(std::declval<Value>(), To<T>{})) > result;
    for (size_t i = 0; i < value.GetNumItems(); i++)
    {
      result.push_back(value[i].template As<T>());
    }
    return result;
  }
}

class mafXMLReaderImpl;
class mafXMLReader
{
public:
	enum PARSER_IO_ERRORS { IO_OK = 0, IO_GENERIC_ERROR, IO_WRONG_OBJECT_TYPE, IO_RESTORE_ERROR, IO_WRONG_FILE_TYPE, IO_WRONG_FILE_VERSION, IO_WRONG_URL, IO_XML_PARSE_ERROR, IO_DOM_XML_ERROR, IO_XML_PARSER_INTERNAL_ERROR, IO_LAST_ERROR };
	mafXMLReader(const mafString& fileType, const mafString& version);
	~mafXMLReader();
	int Load(const mafString& url);
	const mafStorageElement& GetRoot() const;
	const mafString& GetURL() const;
protected:
private:
	std::unique_ptr<mafXMLReaderImpl> m_impl;
};

class MAF_EXPORT mafStorageElementBuilder
{
public:
  static const size_t npos = size_t(-1);

  mafStorageElementBuilder(void* element);
  ~mafStorageElementBuilder();

  mafStorageElementBuilder operator[](const mafString& name);
  mafStorageElementBuilder operator()(const mafString& name);
  mafStorageElementBuilder operator[](size_t idx);

  template<class T>
  int SetValue(const T&);

  bool      IsValid()     const;
  void*     GetImpl()     const;
  size_t    GetNumItems() const;

protected:
  size_t m_NumItems;
  void* m_DOMElement;
};

template<typename T>
int mafStorageElementBuilder::SetValue(const T& val)
{
  using namespace serializer;
  Serialize(*this, val);
  return MAF_OK;
}

namespace serializer
{
  void Serialize(mafStorageElementBuilder& value, const mafString&);
  void Serialize(mafStorageElementBuilder& value, const unsigned int&);
  void Serialize(mafStorageElementBuilder& value, const int&);
  void Serialize(mafStorageElementBuilder& value, const int64_t&);
  void Serialize(mafStorageElementBuilder& value, const uint64_t&);
  void Serialize(mafStorageElementBuilder& value, const double&);
}

class mafXMLWriterImpl;
class mafXMLWriter
{
public:
	enum PARSER_IO_ERRORS { IO_OK = 0, IO_GENERIC_ERROR, IO_WRONG_OBJECT_TYPE, IO_RESTORE_ERROR, IO_WRONG_FILE_TYPE, IO_WRONG_FILE_VERSION, IO_WRONG_URL, IO_XML_PARSE_ERROR, IO_DOM_XML_ERROR, IO_XML_PARSER_INTERNAL_ERROR, IO_LAST_ERROR };
	mafXMLWriter(const mafString& fileType, const mafString& version);
	~mafXMLWriter();
	mafStorageElementBuilder& GetRoot();
	int Save(const mafString& url);
protected:
private:
	std::unique_ptr<mafXMLWriterImpl> m_impl;
};

#endif // _mafStorageElement_h_
