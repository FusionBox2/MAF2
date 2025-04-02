#pragma once
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

#include "mafOp.h"

/** 
  class name: medOpComputeWrapping
  Operation used to create a medVMEComputeWrapping used to wrap muscle action lines.
*/
class MED_OPERATION_EXPORT medOpComputeWrapping: public mafOp
{
public:
  /** constructor */
  medOpComputeWrapping(const mafString& label = _R("CreateWrappedMeter"));
  /** destructor */
  ~medOpComputeWrapping() override;

  /** RTTI macro*/
  mafTypeMacroN(medOpComputeWrapping);

  /** clone the object and retrieve a copy*/
  mafOp* Copy() override;

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node) override;
  /** Builds operation's interface. */
  void OpRun() override;
};
