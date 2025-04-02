#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateVolume :
//----------------------------------------------------------------------------
/** Operation used to create a mafVolumeGray containing the vtkStructuredPoint
dataset filled with a constant scalar value. This is useful for testing purposes.*/
class MAF_EXPORT mafOpCreateOsteometricBoard : public mafOp
{
public:
	mafOpCreateOsteometricBoard(const mafString& label = _R("Create OsteometricBoard"));
	~mafOpCreateOsteometricBoard() override;

	mafTypeMacroN(mafOpCreateOsteometricBoard);

  /** Receive events coming from the user interface.*/
  //void OnEvent(mafEventBase *maf_event);

  /** Return a copy of the operation.*/
  mafOp* Copy() override;

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node) override;

  /** Builds operation's interface. */
  void OpRun() override;
};
