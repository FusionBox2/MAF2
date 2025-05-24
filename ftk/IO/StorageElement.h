#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Meta.h"
#include "ftk/Base/String.h"
#include "ftk/IO/To.h"

#include <vector>
#include <map>
#include <optional>

class mafXMLReader;

class FTK_IO_EXPORT mafStorageElement
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
  template <typename T>
  void CheckInBounds(T x, T min, T max) {
    if (x < min || x > max) {
      throw std::exception("Out of range");
    }
  }
  template <typename Dst, typename Src>
  Dst NarrowToInt(Src x) {
    static_assert(
      std::numeric_limits<Src>::min() <= std::numeric_limits<Dst>::min() &&
      std::numeric_limits<Src>::max() >= std::numeric_limits<Dst>::max(),
      "expanding cast requested"
      );
    CheckInBounds<Src>(x, std::numeric_limits<Dst>::min(), std::numeric_limits<Dst>::max());
    return static_cast<Dst>(x);
  }

	mafString Parse(const mafStorageElement& value, To<mafString>);
  
  template <typename Value, typename T>
  std::enable_if_t<kIsInteger<T>, T> Parse(const Value& value, To<T>) {
    using IntT = std::conditional_t<std::is_signed_v<T>, intmax_t, uintmax_t>;
    return NarrowToInt<T>(value.template As<IntT>());
  }

  intmax_t Parse(const mafStorageElement& value, To<intmax_t>);
  uintmax_t Parse(const mafStorageElement& value, To<uintmax_t>);
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

namespace serializer
{
  void Serialize(mafStorageElementBuilder& value, const mafString&);
  void Serialize(mafStorageElementBuilder& value, const intmax_t&);
  void Serialize(mafStorageElementBuilder& value, const uintmax_t&);
  void Serialize(mafStorageElementBuilder& value, const double&);
  template <typename W, typename T>
  std::enable_if_t<kIsInteger<T> && !std::is_same_v<T, intmax_t> && !std::is_same_v<T, intmax_t>, void> Serialize(W& w, T value) {
    using IntT = std::conditional_t<std::is_signed_v<T>, intmax_t, uintmax_t>;
    Serialize(w, static_cast<IntT>(value));
  }

}

template<typename T>
int mafStorageElementBuilder::SetValue(const T& val)
{
  using namespace serializer;
  Serialize(*this, val);
  return MAF_OK;
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
