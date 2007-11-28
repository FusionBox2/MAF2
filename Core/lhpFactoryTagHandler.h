/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpFactoryTagHandler.h,v $
  Language:  C++
  Date:      $Date: 2007-11-28 16:36:51 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __lhpFactoryTagHandler_h
#define __lhpFactoryTagHandler_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObjectFactory.h"

/** to be used internally for plugging default tags handler --- calls a member function directly */
#define lhpPlugTagHandlerMacro(tag_handler_type,descr) \
  RegisterNewTagHandler(tag_handler_type::GetStaticTypeName(), descr, tag_handler_type::NewObject);
  
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------  

// to be defined in a separate file... this is just an empty skeleton

#include "mafObject.h"
#include "mafObserver.h"

class lhpTagHandler : public mafObject, public mafObserver
  {
  public:
    mafTypeMacro(lhpTagHandler,mafObject);

    lhpTagHandler();
    virtual			~lhpTagHandler();

  protected:
  private:
};

/** Object factory for tag handlers.
  To make a new handler available in the LHDL it must be plugged inside a factory, in particular
  this factory must be of type lhpFactoryTagHandler to be able to retrieve the list of handlers plugged
  in the factory. Also when using lhpPlugTagHandler<tag_handler_type> the pipe icon is plugged inside the
  the MAF picture factory. */

class MAF_EXPORT lhpFactoryTagHandler : public mafObjectFactory
// need to create LHP_EXPORT symbol
{
public: 
  mafTypeMacro(lhpFactoryTagHandler,mafObjectFactory);
  virtual const char* GetMAFSourceVersion() const;
  virtual const char* GetDescription() const;

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();

  /** return the instance pointer of the factory. return NULL if not initialized yet */
  static lhpFactoryTagHandler *GetInstance() {if (!m_Instance) Initialize(); return m_Instance;}

  /** create an instance of the pipe give its type name */
  static lhpTagHandler *CreateTagHandlerInstance(const char *type_name);
   
   /**
    This function can be used by Application code to register new Objects's to the mflCoreFactory */
  void RegisterNewTagHandler(const char* tagHandlerName, const char* description, mafCreateObjectFunction createFunction);

  /** return list of names for pipes plugged into this factory */
  const static std::vector<std::string> &GetPipeNames() {return m_TagHandlerNames;}

protected:
  lhpFactoryTagHandler();
  ~lhpFactoryTagHandler() { }

  static lhpFactoryTagHandler *m_Instance;
  static std::vector<std::string> m_TagHandlerNames; 
  
private:
  lhpFactoryTagHandler(const lhpFactoryTagHandler&);  // Not implemented.
  void operator=(const lhpFactoryTagHandler&);  // Not implemented.
};

/** Plug  a pipe in the main MAF Pipe factory.*/
template <class T>
class MAF_EXPORT lhpPlugTagHandler
// needs to create LHP_EXPORT symbol 
{
  public:
  lhpPlugTagHandler(const char *description);
  
};

//------------------------------------------------------------------------------
/** Plug a new Pipe class into the Pipe factory.*/
template <class T>
lhpPlugTagHandler<T>::lhpPlugTagHandler(const char *description)
//------------------------------------------------------------------------------
{ 
  lhpFactoryTagHandler *factory=lhpFactoryTagHandler::GetInstance();
  if (factory)
  {
    factory->RegisterNewTagHandler(T::GetStaticTypeName(), description, T::NewObject);
  }
}

#endif
