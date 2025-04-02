#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateSurfaceParametric :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateSurfaceParametric: public mafOp
{
public:
  mafOpCreateSurfaceParametric(const mafString& label = _R("Create Parametric Surface"));
  ~mafOpCreateSurfaceParametric() override; 

  mafTypeMacroN(mafOpCreateSurfaceParametric);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
