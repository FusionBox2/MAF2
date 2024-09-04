/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpCreateObject.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpCreateObject_H__
#define __lhpOpCreateObject_H__


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEVolume.h"
#include "mafVMEProber.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// lhpOpCreateObject :
//----------------------------------------------------------------------------
/** */

template <class O>
class lhpOpCreateObject: public mafOp
{
public:
  /*const mafTypeID &GetStaticTypeId() {return typeid(*this);}
  const mafTypeID &GetTypeId() const {return typeid(*this);}
  const char *GetStaticTypeName() {return "lhpOpCreateObject";}
  const char *GetTypeName() const {return "lhpOpCreateObject";}
  MAF_EXPORT bool IsStaticType(const char *type) const { return ( strcmp("lhpOpCreateObject",type)==0 ) ? true : mafOp::IsStaticType(type); }
  MAF_EXPORT bool IsStaticType(const mafTypeID &type) const { return ( type==typeid(*this) ? true : mafOp::IsStaticType(type) ); }
  MAF_EXPORT bool IsA(const char *type) const {return IsStaticType(type);}
  MAF_EXPORT bool IsA(const mafTypeID &type) const {return IsStaticType(type);}
  lhpOpCreateObject* SafeDownCast(mafObject *o){ try { return dynamic_cast<lhpOpCreateObject *>(o); } catch (std::bad_cast) { return NULL;} }
  MAF_EXPORT mafObject *NewObject()const{lhpOpCreateObject *obj = new lhpOpCreateObject("");if (obj) obj->m_HeapFlag=true;return obj;}
  MAF_EXPORT mafObject *NewObjectInstance() const {return NewObject(); }
  MAF_EXPORT lhpOpCreateObject *New(){ return (lhpOpCreateObject *)NewObject(); }
  MAF_EXPORT lhpOpCreateObject *NewInstance() const {return (lhpOpCreateObject *)NewObjectInstance();}
*/
  lhpOpCreateObject(const mafString& name, const mafString& label = "CreateObject"):mafOp(label),m_CreatedName(name)
  {
    m_OpType  = OPTYPE_OP;
    m_Canundo = true;
    m_Created = NULL;
  }
 ~lhpOpCreateObject() 
 {
   mafDEL(m_Created);
 }

  mafOp* Copy(){return new lhpOpCreateObject(m_CreatedName, GetLabel());}

  bool Accept(mafNode *node){return (node != NULL);}
  void OpRun()
  {
    mafNEW(m_Created);
    m_Created->SetName(m_CreatedName);
    m_Output = m_Created;
    {mafEvent evUnq(this,OP_RUN_OK); mafEventMacro(evUnq);}
  }


protected: 
  O         *m_Created;
  mafString  m_CreatedName;
};

#endif
