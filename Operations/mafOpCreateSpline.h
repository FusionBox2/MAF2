#pragma once
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEPolylineSpline;
class mafGUI;
class mafEvent;
//----------------------------------------------------------------------------
// mafOpCreateSpline :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateSpline: public mafOp
{
public:
  mafOpCreateSpline(const mafString& label = _R("Create Parametric Surface"));
  ~mafOpCreateSpline() override; 

  mafTypeMacroN(mafOpCreateSpline);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
