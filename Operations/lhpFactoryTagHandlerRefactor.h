/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpFactoryTagHandlerRefactor.h,v $
  Language:  C++
  Date:      $Date: 2009-03-30 14:25:13 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __lhpFactoryTagHandlerRefactor_h
#define __lhpFactoryTagHandlerRefactor_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObjectFactory.h"

/** to be used internally for plugging default tags handler --- calls a member function directly */
#define lhpPlugTagHandlerMacro(tag_handler_type,descr) \
  RegisterNewTagHandler(tag_handler_type::GetStaticTypeName(), descr, tag_handler_type::NewObject);
  

// to be defined in a separate file. This is just an empty skeleton
// start skeleton
#include "mafObject.h"
#include "lhpTagHandlerRefactor.h"
#include "lhpTagHandlerContainer.h"
#include "lhpTagHandlerDICOMContainer.h"
#include "mafVME.h"

// end skeleton...

/** Object factory for tag handlers.
  To make a new handler available in the LHDL it must be plugged inside a factory, in particular
  this factory must be of type lhpFactoryTagHandlerRefactor to be able to retrieve the list of handlers plugged
  in the factory.  */

class MAF_EXPORT lhpFactoryTagHandlerRefactor : public mafObjectFactory
// need to create LHP_EXPORT symbol
{
public: 
  mafTypeMacro(lhpFactoryTagHandlerRefactor,mafObjectFactory);
  virtual const char* GetMAFSourceVersion() const;
  virtual const char* GetDescription() const;

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();

  /** return the instance pointer of the factory. return NULL if not initialized yet */
  static lhpFactoryTagHandlerRefactor *GetInstance() {if (!m_Instance) Initialize(); return m_Instance;}

  /** create an instance of the tag handler give its type name */
  static lhpTagHandlerRefactor *CreateTagHandlerInstance(const char *type_name);
   
   /**
    This function can be used by Application code to register new Objects's to the mflCoreFactory */
  void RegisterNewTagHandler(const char* tagHandlerName, const char* description, mafCreateObjectFunction createFunction);

  /** return list of names for tag handlers plugged into this factory */
  const static std::vector<std::string> &GetTagHandlerNames() {return m_TagHandlerNames;}

protected:
  lhpFactoryTagHandlerRefactor();
  ~lhpFactoryTagHandlerRefactor() { }

  static lhpFactoryTagHandlerRefactor *m_Instance;
  static std::vector<std::string> m_TagHandlerNames; 
  
private:
  lhpFactoryTagHandlerRefactor(const lhpFactoryTagHandlerRefactor&);  // Not implemented.
  void operator=(const lhpFactoryTagHandlerRefactor&);  // Not implemented.
};

/** Plug  a tag handler in the main tag handlers factory.*/
template <class T>
class MAF_EXPORT lhpPlugTagHandler
// needs to create LHP_EXPORT symbol 
{
  public:
  lhpPlugTagHandler(const char *description);
  
};

//------------------------------------------------------------------------------
/** Plug a new tag handler class into the tag handlers factory.*/
template <class T>
lhpPlugTagHandler<T>::lhpPlugTagHandler(const char *description)
//------------------------------------------------------------------------------
{ 
  lhpFactoryTagHandlerRefactor *factory=lhpFactoryTagHandlerRefactor::GetInstance();
  if (factory)
  {
    factory->RegisterNewTagHandler(T::GetStaticTypeName(), description, T::NewObject);
  }
}

#endif
