#pragma once
#include "mafOp.h"

//----------------------------------------------------------------------------
// lhpOpCreateSurfaceScalar :
//----------------------------------------------------------------------------
/** Operation used to create a new lhpVMESurfaceScalarVarying.
@sa lhpVMESurfaceScalarVarying lhpVisualPipeSurfaceScalar*/
class lhpOpCreateSurfaceScalar: public mafOp
{
public:
  lhpOpCreateSurfaceScalar(const mafString& label = _R("Create surface scalar"));
  ~lhpOpCreateSurfaceScalar() override; 

  mafTypeMacroN(lhpOpCreateSurfaceScalar);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
