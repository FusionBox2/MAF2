#pragma once

#include "ftkConfigure.h"

#include <ftk/Base/FastPImpl.h>
#include "ftk/Base/Meta.h"
#include "ftk/Base/String.h"
#include "ftk/IO/Parse.h"
#include "ftk/IO/Serialize.h"

BEGIN_FTK_NAMESPACE

namespace io
{
  class Reader;
}

class FTK_IO_EXPORT mafStorageElement
{
public:
  struct DefaultConstructed {};

	mafStorageElement();
  mafStorageElement(const mafStorageElement&);
  mafStorageElement(mafStorageElement&&) noexcept;
  mafStorageElement& operator=(const mafStorageElement&);
  mafStorageElement& operator=(mafStorageElement&&) noexcept;
  ~mafStorageElement();

	mafStorageElement operator[](const mafString& name) const;
  mafStorageElement operator()(const mafString& name) const;
  mafStorageElement operator[](std::string_view name) const;
  mafStorageElement operator()(std::string_view name) const;
  mafStorageElement operator[](size_t idx) const;

	template<typename T>
  auto As() const;

	template <typename T, typename First, typename... Rest>
  auto As(First&& default_arg, Rest&&... more_default_args) const;

  /// @brief Returns value of *this converted to T or T() if this->IsMissing().
  /// @throw Anything derived from std::exception.
  /// @note Use as `value.As<T>({})`
  template <typename T>
  auto As(DefaultConstructed) const;

  bool      isValid()     const;

  size_t    size() const;

  io::Reader* GetStorage() const;

//private:
  class StorageElemImpl;
  utilities::FastPImpl<StorageElemImpl, 80, 8> m_impl;

	friend class mafXMLReader;
};

template<typename T>
auto mafStorageElement::As() const
{
  return Parse(*this, io::parse::To<T>{});
}

template <typename T, typename First, typename... Rest>
auto mafStorageElement::As(First&& default_arg, Rest&&... more_default_args) const {
  if (isValid())
  {
    // intended raw ctor call, sometimes casts
    // NOLINTNEXTLINE(google-readability-casting)
    return decltype(As<T>())(std::forward<First>(default_arg), std::forward<Rest>(more_default_args)...);
  }
  return As<T>();
}

template <typename T>
auto mafStorageElement::As(mafStorageElement::DefaultConstructed) const {
  return isValid() ? decltype(As<T>())() : As<T>();
}

namespace io::parse
{
  std::string Parse(const mafStorageElement& value, To<std::string>);
  mafString Parse(const mafStorageElement& value, To<mafString>);
  intmax_t Parse(const mafStorageElement& value, To<intmax_t>);
  uintmax_t Parse(const mafStorageElement& value, To<uintmax_t>);
  double Parse(const mafStorageElement& value, To<double>);
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

  mafStorageElementBuilder();
  mafStorageElementBuilder(const mafStorageElementBuilder&);
  mafStorageElementBuilder(mafStorageElementBuilder&&) noexcept;
  mafStorageElementBuilder& operator=(const mafStorageElementBuilder&);
  mafStorageElementBuilder& operator=(mafStorageElementBuilder&&) noexcept;
  ~mafStorageElementBuilder();

  mafStorageElementBuilder operator[](const mafString& name);
  mafStorageElementBuilder operator()(const mafString& name);
  mafStorageElementBuilder operator[](std::string_view name);
  mafStorageElementBuilder operator()(std::string_view name);
  mafStorageElementBuilder operator[](size_t idx);

  template<class T>
  int SetValue(const T&);

  bool      isValid()     const;
  size_t    size() const;

//protected:
  class StorageElemBuildImpl;
  utilities::FastPImpl<StorageElemBuildImpl, 72, 8> m_impl;
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
