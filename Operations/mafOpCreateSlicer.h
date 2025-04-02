#pragma once

#include "mafOp.h"
#include "mafVMEVolume.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESlicer;

//----------------------------------------------------------------------------
// mafOpCreateSlicer :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateSlicer: public mafOp
{
public:
  mafOpCreateSlicer(const mafString& label = _R("CreateSlicer"));
 ~mafOpCreateSlicer() override; 

  mafTypeMacroN(mafOpCreateSlicer);

  mafOp* Copy() override;

  bool Accept(mafNode *node) override;
  void OpRun() override;
  void OpDo() override;

  static bool VolumeAccept(mafNode* node) {return(node != nullptr  && node->IsMAFType(mafVMEVolume));};

protected: 
  mafNode      *m_SlicedVME = nullptr;
};
