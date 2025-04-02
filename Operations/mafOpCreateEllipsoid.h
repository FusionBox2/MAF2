#pragma once
#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateSurfaceParametric :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateEllipsoid: public mafOp
{
public:
  mafOpCreateEllipsoid(const mafString& label = _R("Create Ellipsoid"));
  ~mafOpCreateEllipsoid() override; 

  mafTypeMacroN(mafOpCreateEllipsoid);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
