#pragma once

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafOp.h"

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
  lhpOpCreateObject(const mafString& name, const mafString& label = _R("CreateObject")):mafOp(label),m_CreatedName(name)
  {
    m_OpType  = OPTYPE_OP;
    m_Canundo = true;
  }
 ~lhpOpCreateObject() override
 {
 }

  mafOp* Copy() override {return new lhpOpCreateObject(m_CreatedName, GetLabel());}

  bool Accept(mafNode *node) override {return (node != nullptr);}
  void OpRun() override
  {
    auto created = O::NewSPtr();
    created->SetName(m_CreatedName);
    SetOutput(created);
    {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
  }
protected: 
  mafString  m_CreatedName;
};
