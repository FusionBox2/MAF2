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
class MAF_EXPORT mafObject
{
public:
  mafObject();
  virtual ~mafObject();

  /** the same as delete obj, implemented for syntax compatibility */
  virtual void Delete() {delete this;};
  
  /** Return the name of this type (static function) */
  static const char *GetStaticTypeName();

  /** Return the class name of this instance */
  virtual const char *GetTypeName() const; 
  
  /** This is used by IsA to check the class name */
  static bool IsStaticType(const char *type_name);
  
  /** This is used by IsA to check the class type id */
  static bool IsStaticType(const mafTypeID &type_id);

  /** Check the class name of this instance */
  virtual bool IsA(const char *type_name) const;
  
  /** Check the type id of this instance */
  virtual bool IsA(const mafTypeID &type_id) const;

  /** Return a new instance of the same type */
  virtual mafObject *NewObjectInstance() const = 0;

  /** Return TypeId for this type (static function, i.e. the pointer type) */ 
  static const mafTypeID &GetStaticTypeId();

  /** Return TypeId for this object instance (the real type) */ 
  virtual const mafTypeID &GetTypeId() const;

  /** Cast with dynamic type checking. This is used for casting from (mafObject *) */
  static mafObject* SafeDownCast(mafObject *o);

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
