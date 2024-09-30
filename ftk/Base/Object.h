#pragma once

#include "ftkConfigure.h"

#include "mafDefines.h"

#include <memory>

#define mafCommonTypeMacro(className) \
  using Thisclass = className; \
  using Pointer = Thisclass*; \
  using ConstPointer = const Thisclass*; \
  using PointerArg = Pointer; \
  using ConstPointerArg = ConstPointer; \
  using UPtr = std::unique_ptr<Thisclass>; \
  using CUPtr = std::unique_ptr<const Thisclass>; \
  using SPtr = std::unique_ptr<Thisclass>; \
  using CSPtr = std::unique_ptr<const Thisclass>; \
  static const mafTypeID& GetStaticTypeId() { return typeid(Thisclass); } \
  static const char *GetStaticTypeName() { return #className; }\
  static Pointer SafeDownCast(Rootclass::Pointer o) { return dynamic_cast<Pointer>(o); } \
  static ConstPointer SafeDownCast(Rootclass::ConstPointer o) { return dynamic_cast<ConstPointer>(o); } \
  static std::shared_ptr<Thisclass> SafeDownCast(std::shared_ptr<Rootclass> o) { return std::dynamic_pointer_cast<Thisclass>(o); }\
  static Pointer StaticDownCast(Rootclass::Pointer o) { return static_cast<Pointer>(o); } \
  static ConstPointer StaticDownCast(Rootclass::ConstPointer o) { return static_cast<ConstPointer>(o); } \
  static std::shared_ptr<Thisclass> StaticDownCast(std::shared_ptr<Rootclass> o) { return std::static_pointer_cast<Thisclass>(o); }

#define mafNewTypeMacro \
  friend class std::unique_ptr<Thisclass>; \
  friend class std::unique_ptr<Rootclass>; \
  friend class std::shared_ptr<Thisclass>; \
  friend class std::shared_ptr<Rootclass>; \
  friend class std::default_delete<Thisclass>; \
  friend class std::default_delete<Rootclass>; \
  static Rootclass::Pointer NewObject() { return New(); } \
  static std::unique_ptr<Rootclass> NewObjectU() { return NewUPtr(); } \
  static std::shared_ptr<Rootclass> NewObjectS() { return NewSPtr(); } \
  static Pointer New() { auto obj = new Thisclass; obj->m_HeapFlag = true; return obj; } \
  static std::unique_ptr<Thisclass> NewUPtr() { return std::unique_ptr<Thisclass>(New()); } \
  static std::shared_ptr<Thisclass> NewSPtr() { return NewUPtr(); }

#define mafNewAbstractTypeMacro \
private: \
  friend class std::unique_ptr<Thisclass>; \
  friend class std::unique_ptr<Rootclass>; \
  friend class std::shared_ptr<Thisclass>; \
  friend class std::shared_ptr<Rootclass>; \
  friend class std::default_delete<Thisclass>; \
  friend class std::default_delete<Rootclass>; \
  static Rootclass::Pointer NewObject() { return New(); } \
  static std::unique_ptr<Rootclass> NewObjectU() { return NewUPtr(); } \
  static std::shared_ptr<Rootclass> NewObjectS() { return NewSPtr(); } \
  static Pointer New() { throw std::logic_error("called function New() in abstract class"); return nullptr; } \
  static std::unique_ptr<Thisclass> NewUPtr() { return std::unique_ptr<Thisclass>(New()); } \
  static std::shared_ptr<Thisclass> NewSPtr() { return NewUPtr(); } \
public:

#define mafAbstractBaseTypeMacro(className) \
public: \
  using Rootclass = className; \
  mafCommonTypeMacro(className) \
  static bool IsStaticType(const char *type) { return strcmp(type, GetStaticTypeName()) == 0; }; \
  static bool IsStaticType(const mafTypeID& type) { return type == GetStaticTypeId(); } \
  virtual const mafTypeID& GetTypeId() const { return GetStaticTypeId(); } \
  virtual const char *GetTypeName() const { return GetStaticTypeName(); } \
  virtual bool IsA(const char *type) const { return IsStaticType(type); } \
  virtual bool IsA(const mafTypeID& type) const { return IsStaticType(type); } \
  virtual Pointer NewInstance() const = 0; \
  mafNewAbstractTypeMacro

#define mafBaseTypeMacro(className) \
public: \
  using Rootclass = className; \
  mafCommonTypeMacro(className) \
  static bool IsStaticType(const char *type) { return strcmp(type, GetStaticTypeName()) == 0; }; \
  static bool IsStaticType(const mafTypeID& type) { return type == GetStaticTypeId(); } \
  virtual const mafTypeID& GetTypeId() const { return GetStaticTypeId(); } \
  virtual const char *GetTypeName() const { return GetStaticTypeName(); } \
  virtual bool IsA(const char *type) const { return IsStaticType(type); } \
  virtual bool IsA(const mafTypeID& type) const { return IsStaticType(type); } \
  virtual Pointer NewInstance() const { return Thisclass::New(); } \
  mafNewTypeMacro

#define mafAbstractTypeMacroN(className) \
public: \
  using Superclass = Thisclass; \
  mafCommonTypeMacro(className) \
  static bool IsStaticType(const char *type) { return strcmp(type, GetStaticTypeName()) == 0 || Superclass::IsStaticType(type); }; \
  static bool IsStaticType(const mafTypeID& type) { return type == GetStaticTypeId() || Superclass::IsStaticType(type); } \
  const mafTypeID& GetTypeId() const override { return GetStaticTypeId(); } \
  const char *GetTypeName() const override { return GetStaticTypeName(); } \
  bool IsA(const char *type) const override { return IsStaticType(type); } \
  bool IsA(const mafTypeID& type) const override { return IsStaticType(type); } \
  Pointer NewInstance() const override = 0 ; \
  mafNewAbstractTypeMacro

#define mafAbstractTypeMacro(className, superclass) \
  mafAbstractTypeMacroN(className)

#define mafTypeMacroN(className) \
public: \
  using Superclass = Thisclass; \
  mafCommonTypeMacro(className) \
  static bool IsStaticType(const char *type) { return strcmp(type, GetStaticTypeName()) == 0 || Superclass::IsStaticType(type); }; \
  static bool IsStaticType(const mafTypeID& type) { return type == GetStaticTypeId() || Superclass::IsStaticType(type); } \
  const mafTypeID& GetTypeId() const override { return GetStaticTypeId(); } \
  const char *GetTypeName() const override { return GetStaticTypeName(); } \
  bool IsA(const char *type) const override { return IsStaticType(type); } \
  bool IsA(const mafTypeID& type) const override { return IsStaticType(type); } \
  Pointer NewInstance() const override {return Thisclass::New(); }; \
  mafNewTypeMacro
  
#define mafTypeMacro(className, superclass) \
  mafTypeMacroN(className)

#define mafCxxAbstractTypeMacro(className)
#define mafCxxTypeMacro(className)

BEGIN_FTK_NAMESPACE

class FTK_BASE_EXPORT mafObject
{
public:
  mafBaseTypeMacro(mafObject)
  mafObject();
  virtual ~mafObject();

  /** return a new instance of the mafObject instance */
  mafObject *NewObjectInstance() const {return NewInstance();}
  /** the same as delete obj, implemented for syntax compatibility */
  virtual void Delete() {delete this;};
  
  /** print debug information for this object */
  virtual void Print(std::ostream &os, const int indent=0) const;

  mafObject(const mafObject& c) {}

#ifndef _DEBUG
	#ifdef _WIN32
		// avoid dll boundary problems
	  void* operator new( size_t tSize );
	  void operator delete( void* p );
	#endif
#endif

protected:
  bool m_HeapFlag; ///< Internally used to mark objects created on the Heap with New()
};

END_FTK_NAMESPACE
