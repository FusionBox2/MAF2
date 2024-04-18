/*=========================================================================

 Program: MAF2
 Module: mafSmartPointer
 Authors: based on vtkSmartPointer (www.vtk.org), rewritten by Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafSmartPointer_h
#define __mafSmartPointer_h

#include "mafConfigure.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafReferenceCounted;

/** Hold a reference to a T instance.
  mafAutoPointer stores a pointer to a mafSmartObjct, and keeps it registered. When 
  allocated on the stack this object allows to automatically unregister an object 
  when exiting from the scope. */
template <class T>
class mafAutoPointer
{
public:  
  /** Initialize smart pointer to (optional) given object. */
	mafAutoPointer(T* r=NULL,void *owner=NULL){
		m_Object=r;
		Register(owner);
	}

  
  /**
    Initialize smart pointer with a new reference to the same object
    referenced by given smart pointer.*/
	mafAutoPointer(const mafAutoPointer& r){m_Object=r.m_Object;Register(NULL);}
  
  /** Destroy smart pointer and remove the reference to its object. */
	~mafAutoPointer(){UnRegister(NULL);}
  
  /**
    Assign object to reference.  This removes any reference to an old
    object.*/
	mafAutoPointer& operator=(T* r){
		// make use of a temp auto-ptr to safely unregister, this
		// to avoid deallocation in case of self assignment
		mafAutoPointer(r).Swap(*this); 
		return *this;
	}

	mafAutoPointer& operator=(const mafAutoPointer<T>& r){
		// make use of a temp auto-ptr to safely unregister, this
		// to avoid deallocation in case of self assignment
		mafAutoPointer<T> tmp(r);
		tmp.Swap(*this); // swaps the two pointers
		return *this;
	}

	template <class U>
  bool operator==(const mafAutoPointer<U>& r) const {return r.m_Object==m_Object;}

  /** Get the contained pointer. */
  T* GetPointer() const {return m_Object;}

  /**
    Allow to pass on the smart pointer to any function requiring a "T *". */
  operator T *() const {return (T *)m_Object;}
  
  /** Provides normal pointer target member access using operator ->. */
  T* operator->() const { return static_cast<T*>(this->m_Object); }

  /** used to force releasing of internal object, specifying the owner */
	void UnRegister(void *owner){
		if(m_Object)
		{
			m_Object->UnRegister(owner);
			m_Object = NULL;
		}
	}
  
protected:
  // Internal utility methods.
	void Swap(mafAutoPointer& r){
		T* temp = r.m_Object;
		r.m_Object = m_Object;
		m_Object = temp;
	}
	void Register(void *owner){
		if(m_Object)
		{
			m_Object->Register(owner);
		}
	}

  T* m_Object; ///<  Pointer to the actual object.
};

/** AutoPointer which self allocates the internal object.
  mafSmartPointer is a specialization of mafAutoPointer which automatically
  instantiate an object in the default constructor. */
template <class T>
class mafSmartPointer: public mafAutoPointer<T>
{
public:
  /**
  Initialize smart pointer to a new instance of class T.*/
  mafSmartPointer() {this->m_Object=T::New();this->Register(NULL);}

  /**
  Initialize smart pointer to given object pointer and reference the given object.*/
  mafSmartPointer(T* r): mafAutoPointer<T>(r) {;}
  
  /**
  Initialize smart pointer with a new reference to the same object
  referenced by given smart pointer.*/
  mafSmartPointer(const mafAutoPointer<T>& r): mafAutoPointer<T>(r) {;}
};


#endif

