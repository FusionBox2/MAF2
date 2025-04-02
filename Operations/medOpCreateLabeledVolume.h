#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// medOpCreateLabeledVolume :
//----------------------------------------------------------------------------
/** 
class name medOpCreateLabeledVolume
Create a medVMELabeledVolume.
*/
class MED_OPERATION_EXPORT medOpCreateLabeledVolume: public mafOp
{
public:
  /** constructor */
  medOpCreateLabeledVolume(const mafString& label = _R("Create labeled volume"));
  /** destructor */
  ~medOpCreateLabeledVolume() override; 

  /** RTTI macro */
  mafTypeMacro(medOpCreateLabeledVolume, mafOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
  mafOp* Copy() override;

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node) override;

  /** Builds operation's interface. */
  void OpRun() override;
};
