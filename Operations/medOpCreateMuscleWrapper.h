#pragma once
#include "mafOp.h"

//----------------------------------------------------------------------------
// medOpCreateMuscleWrapper :
//----------------------------------------------------------------------------
/** */
class medOpCreateMuscleWrapper: public mafOp
{
public:
  medOpCreateMuscleWrapper(const mafString& label = _R("CreateMuscleWrappedMeter"));
  ~medOpCreateMuscleWrapper() override; 

  mafTypeMacroN(medOpCreateMuscleWrapper);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
};
