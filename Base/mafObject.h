/*=========================================================================

 Program: MAF2
 Module: mafObject
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafObject_h
#define __mafObject_h

#include "mafDefines.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// mafObject
//------------------------------------------------------------------------------
/** Abstract superclass for all MAF classes implementing RTTI APIs.
  mafObject is a superclass implementing some useful features like 
  RTTI APIs. MAF objects can be created either on the stack or dynamically
  in the heap. The the latter you can use both "new" or "::New()" for instantiacion
  and "delete" or "Delete()" for deletion. There's no difference between the two.
  For reference counted objects these two methods behave differently.
  @sa mafReferenceCounted
*/

#define mafAbstractBaseTypeMacro(thisClass) \
  /** return the class type id for this class type (static function) */ \
  static const mafTypeID &GetStaticTypeId(); \
  /** return the class type id for this mafObject instance */ \
  virtual const mafTypeID &GetTypeId() const; \
  /** Return the name of this type (static function) */ \
  static const char *GetStaticTypeName(); \
  /** Return the class name of this instance */ \
  virtual const char *GetTypeName() const; \
  /** This is used by IsA to check the class name */ \
  static bool IsStaticType(const char *type); \
  /** This is used by IsA to check the class type id */ \
  static bool IsStaticType(const mafTypeID &type); \
  /** Check the class name of this instance */ \
  virtual bool IsA(const char *type) const; \
  /** Check the type id of this instance */ \
  virtual bool IsA(const mafTypeID &type) const; \
  /** Cast with dynamic type checking. This is used for casting from a (mafObject *) */ \
  static thisClass* SafeDownCast(mafObject *o); \
  /** return a typed new instance of a given object (this calls NewObject and casts) */ \
  virtual thisClass *NewInstance() const;

/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the class definition public section. */
#define mafConcreteBaseTypeMacro(thisClass) \
  /** return a new instance of the given type (static function) */  \
  static mafObject *NewObject(); \
  /** return a new instance of the this class type (static function). It can be called with "object_type::New()"  \
Also this function must be used for creating objects to be used with reference counting in place of the new() \
operator. */ \
  static thisClass *New();

/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the class definition public section. */
#define mafBaseTypeMacro(thisClass) \
  /** commodity type representing the parent class type */ \
  mafAbstractBaseTypeMacro(thisClass) \
  mafConcreteBaseTypeMacro(thisClass)


/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the class definition public section. */
#define mafAbstractTypeMacro(thisClass,superclass) \
  public: \
  /** commodity type representing the parent class type */ \
  typedef superclass Superclass; \
  mafAbstractBaseTypeMacro(thisClass) \

/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the class definition public section. */
#define mafTypeMacro(thisClass,superclass) \
  /** commodity type representing the parent class type */ \
  mafAbstractTypeMacro(thisClass, superclass) \
  mafConcreteBaseTypeMacro(thisClass)



/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the class definition public section. */
#define mafAbstractTypeI2Macro(thisClass,superclass1,superclass2) \
  public: \
  /** commodity type representing the parent class type */ \
  typedef superclass1 Superclass1; \
  typedef superclass2 Superclass2; \
  mafAbstractBaseTypeMacro(thisClass)

/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the class definition public section. */
#define mafTypeI2Macro(thisClass,superclass1,superclass2) \
  mafAbstractTypeI2Macro(thisClass,superclass1,superclass2) \
  mafConcreteBaseTypeMacro(thisClass)



/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the class definition public section. */
#define mafAbstractTypeI3Macro(thisClass,superclass1,superclass2,superclass3) \
  public: \
  /** commodity type representing the parent class type */ \
  typedef superclass1 Superclass1; \
  typedef superclass2 Superclass2; \
  typedef superclass3 Superclass3; \
  mafAbstractBaseTypeMacro(thisClass)

/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the class definition public section. */
#define mafTypeI3Macro(thisClass,superclass1,superclass2,superclass3) \
  mafAbstractTypeI3Macro(thisClass,superclass1,superclass2,superclass3) \
  mafConcreteBaseTypeMacro(thisClass)

/**
Macro used by mafObjects for RTTI information. This macor must be placed
in the .cpp file. */
#define mafCxxAbstractCommonBaseTypeMacro(thisClass) \
  const mafTypeID &thisClass::GetStaticTypeId() {return typeid(thisClass);} \
  const mafTypeID &thisClass::GetTypeId() const {return typeid(thisClass);} \
  const char *thisClass::GetStaticTypeName() {return #thisClass;} \
  const char *thisClass::GetTypeName() const {return #thisClass;} \
  bool thisClass::IsA(const char *type) const {return IsStaticType(type);} \
  bool thisClass::IsA(const mafTypeID &type) const {return IsStaticType(type);} \
  thisClass* thisClass::SafeDownCast(mafObject *o) \
  { try { return dynamic_cast<thisClass *>(o); } catch (std::bad_cast) { return NULL;} }


/**
Macro used by mafObjects for RTTI information. This macor must be placed
in the .cpp file. */
#define mafCxxAbstractBaseTypeI0Macro(thisClass) \
  mafCxxAbstractCommonBaseTypeMacro(thisClass) \
  bool thisClass::IsStaticType(const char *type) \
  { return ( strcmp(#thisClass,type)==0 );} \
  bool thisClass::IsStaticType(const mafTypeID &type) \
  { return ( type==typeid(thisClass));}

/**
Macro used by mafObjects for RTTI information. This macor must be placed
in the .cpp file. */
#define mafCxxAbstractBaseTypeMacro(thisClass) \
  mafCxxAbstractCommonBaseTypeMacro(thisClass) \
  bool thisClass::IsStaticType(const char *type) \
  { return ( strcmp(#thisClass,type)==0 ) ? true : Superclass::IsStaticType(type); } \
  bool thisClass::IsStaticType(const mafTypeID &type) \
  { return ( type==typeid(thisClass) ? true : Superclass::IsStaticType(type) ); }

/**
Macro used by mafObjects for RTTI information. This macor must be placed
in the .cpp file. */
#define mafCxxAbstractBaseTypeI2Macro(thisClass) \
  mafCxxAbstractCommonBaseTypeMacro(thisClass) \
  bool thisClass::IsStaticType(const char *type) \
  { return ( strcmp(#thisClass,type)==0 ) ? true : (Superclass1::IsStaticType(type) || Superclass2::IsStaticType(type)); } \
  bool thisClass::IsStaticType(const mafTypeID &type) \
  { return ( type==typeid(thisClass) ? true : (Superclass1::IsStaticType(type) || Superclass2::IsStaticType(type)) ); }

/**
Macro used by mafObjects for RTTI information. This macor must be placed
in the .cpp file. */
#define mafCxxAbstractBaseTypeI3Macro(thisClass) \
  mafCxxAbstractCommonBaseTypeMacro(thisClass) \
  bool thisClass::IsStaticType(const char *type) \
  { return ( strcmp(#thisClass,type)==0 ) ? true : (Superclass1::IsStaticType(type) || Superclass2::IsStaticType(type) || Superclass3::IsStaticType(type)); } \
  bool thisClass::IsStaticType(const mafTypeID &type) \
  { return ( type==typeid(thisClass) ? true : (Superclass1::IsStaticType(type) || Superclass2::IsStaticType(type) || Superclass3::IsStaticType(type)) ); }


/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the .cpp file. */
#define mafCxxConcreteBaseTypeMacro(thisClass) \
  mafObject *thisClass::NewObject() \
  { return New(); } \
  thisClass *thisClass::New() \
  { \
  thisClass *obj = new thisClass; \
  if (obj) obj->m_HeapFlag=true; \
  return obj; \
  } \
  thisClass *thisClass::NewInstance() const {return New();}


/**
Macro used by mafObjects for RTTI information. This macor must be placed
in the .cpp file. */
#define mafCxxAbstractTypeI0Macro(thisClass) \
  mafCxxAbstractBaseTypeI0Macro(thisClass) \
  /** return a typed new instance of a given object (this calls NewObject and casts) */ \
  thisClass *thisClass::NewInstance() const {return NULL;}

/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the .cpp file. */
#define mafCxxTypeI0Macro(thisClass) \
  mafCxxAbstractBaseTypeI0Macro(thisClass) \
  mafCxxConcreteBaseTypeMacro(thisClass)

/**
Macro used by mafObjects for RTTI information. This macor must be placed
in the .cpp file. */
#define mafCxxAbstractTypeMacro(thisClass) \
  mafCxxAbstractBaseTypeMacro(thisClass) \
  /** return a typed new instance of a given object (this calls NewObject and casts) */ \
  thisClass *thisClass::NewInstance() const {return NULL;}

/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the .cpp file. */
#define mafCxxTypeMacro(thisClass) \
  mafCxxAbstractBaseTypeMacro(thisClass) \
  mafCxxConcreteBaseTypeMacro(thisClass)

/**
Macro used by mafObjects for RTTI information. This macor must be placed
in the .cpp file. */
#define mafCxxAbstractTypeI2Macro(thisClass) \
  mafCxxAbstractBaseTypeI2Macro(thisClass) \
  /** return a typed new instance of a given object (this calls NewObject and casts) */ \
  thisClass *thisClass::NewInstance() const {return NULL;}

/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the .cpp file. */
#define mafCxxTypeI2Macro(thisClass) \
  mafCxxAbstractBaseTypeI2Macro(thisClass) \
  mafCxxConcreteBaseTypeMacro(thisClass)

/**
Macro used by mafObjects for RTTI information. This macor must be placed
in the .cpp file. */
#define mafCxxAbstractTypeI3Macro(thisClass) \
  mafCxxAbstractBaseTypeI3Macro(thisClass) \
  /** return a typed new instance of a given object (this calls NewObject and casts) */ \
  thisClass *thisClass::NewInstance() const {return NULL;}

/**
Macro used by mafObjects for RTTI information. This macro must be placed
in the .cpp file. */
#define mafCxxTypeI3Macro(thisClass) \
  mafCxxAbstractBaseTypeI3Macro(thisClass) \
  mafCxxConcreteBaseTypeMacro(thisClass)


class MAF_EXPORT mafObject
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
#endif /* __mafObject_h */
