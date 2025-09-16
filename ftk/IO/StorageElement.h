#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Meta.h"
#include "ftk/Base/String.h"
#include "ftk/IO/Parse.h"
#include "ftk/IO/ParseTo.h"
#include "ftk/IO/Serialize.h"

#include <vector>
#include <map>
#include <optional>

BEGIN_FTK_NAMESPACE

namespace io
{
  class Reader;
}

class FTK_IO_EXPORT mafStorageElement
{
public:
  mafStorageElement(void* element, io::Reader* storage);
  mafStorageElement(void* const * elements, size_t numElems, io::Reader* storage);
  ~mafStorageElement();

  mafString GetName() const;

  mafStorageElement operator[](const mafString& name) const;
  mafStorageElement operator()(const mafString& name) const;
  mafStorageElement operator[](size_t idx) const;

  template<typename T>
  auto As() const;

  io::Reader* GetStorage()  const {return m_Storage;}

  mafString UpgradeAttribute(const mafString& attribute) const;

  bool      IsValid()     const;
  bool      isValid() const;
  void*     GetImpl()     const;
  size_t    GetNumItems() const;
  size_t    size() const;

protected:
  void*                                     m_DOMElement;
  size_t                                    m_NumItems;
  io::Reader*                             m_Storage;
  std::map<mafString, std::vector<void*> >  m_Children;
};

template<typename T>
auto mafStorageElement::As() const
{
  return Parse(*this, io::parse::To<T>{});
}

namespace io::parse
{
	mafString Parse(const mafStorageElement& value, io::parse::To<mafString>);
	intmax_t Parse(const mafStorageElement& value, io::parse::To<intmax_t>);
	uintmax_t Parse(const mafStorageElement& value, io::parse::To<uintmax_t>);
	double Parse(const mafStorageElement& value, io::parse::To<double>);
}

namespace io
{
  class ReaderImpl;
  class FTK_IO_EXPORT Reader
  {
  public:
    enum PARSER_IO_ERRORS { IO_OK = 0, IO_GENERIC_ERROR, IO_WRONG_OBJECT_TYPE, IO_RESTORE_ERROR, IO_WRONG_FILE_TYPE, IO_WRONG_FILE_VERSION, IO_WRONG_URL, IO_XML_PARSE_ERROR, IO_DOM_XML_ERROR, IO_XML_PARSER_INTERNAL_ERROR, IO_LAST_ERROR };

  	Reader(const mafString& fileType, const mafString& version);

  	Reader() = delete;
    Reader(const Reader&) = delete;
    Reader(Reader&&) noexcept = delete;
    Reader& operator=(const Reader&) = delete;
    Reader& operator=(Reader&&) noexcept = delete;
    ~Reader();

  	int Load(const mafString& url);

  	mafStorageElement GetRoot() const;

  	const mafString& GetURL() const;

  private:
    std::unique_ptr<ReaderImpl> m_impl;
  };
}

class FTK_IO_EXPORT mafStorageElementBuilder
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
  size_t    size() const;

protected:
  size_t m_NumItems;
  void* m_DOMElement;
};

namespace serializer
{
  void Serialize(mafStorageElementBuilder& value, const std::string&);
  void Serialize(mafStorageElementBuilder& value, const mafString&);
  void Serialize(mafStorageElementBuilder& value, const intmax_t&);
  void Serialize(mafStorageElementBuilder& value, const uintmax_t&);
  void Serialize(mafStorageElementBuilder& value, const double&);
  template <typename W, typename T>
  std::enable_if_t<meta::isInteger<T> && !std::is_same_v<T, intmax_t> && !std::is_same_v<T, intmax_t>, void> Serialize(W& w, T value) {
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

namespace io
{
  class WriterImpl;
  class FTK_IO_EXPORT Writer
  {
  public:
    enum PARSER_IO_ERRORS { IO_OK = 0, IO_GENERIC_ERROR, IO_WRONG_OBJECT_TYPE, IO_RESTORE_ERROR, IO_WRONG_FILE_TYPE, IO_WRONG_FILE_VERSION, IO_WRONG_URL, IO_XML_PARSE_ERROR, IO_DOM_XML_ERROR, IO_XML_PARSER_INTERNAL_ERROR, IO_LAST_ERROR };
    Writer(const mafString& fileType, const mafString& version);

    Writer() = delete;
    Writer(const Writer&) = delete;
    Writer(Writer&&) noexcept = delete;
    Writer& operator=(const Writer&) = delete;
    Writer& operator=(Writer&&) noexcept = delete;
    ~Writer();

  	mafStorageElementBuilder GetRoot();

  	int Save(const mafString& url) const;

  private:
    std::unique_ptr<WriterImpl> m_impl;
  };
}

END_FTK_NAMESPACE
