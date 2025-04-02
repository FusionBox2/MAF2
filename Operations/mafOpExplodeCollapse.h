#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// mafOpExplodeCollapse :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpExplodeCollapse: public mafOp
{
public:
  mafOpExplodeCollapse(const mafString& label = _R("ExplodeCollapse"));
 ~mafOpExplodeCollapse() override; 
  
  mafTypeMacroN(mafOpExplodeCollapse);

  mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node) override;

	/** Builds operation's interface. */
  void OpRun() override;

	/** Execute the operation. */
  void OpDo() override;

	/** Makes the undo for the operation. */
  void OpUndo() override;
};
