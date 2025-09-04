#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMELandmarkCloud;
//----------------------------------------------------------------------------
// lhpOpCreateRefSysLM :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT lhpOpCreateRefSysLM: public mafOp
{
public:
  lhpOpCreateRefSysLM(const mafString& label = _R("Create RefSys Landmarks"));
  ~lhpOpCreateRefSysLM() override; 

  mafTypeMacroN(lhpOpCreateRefSysLM);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
