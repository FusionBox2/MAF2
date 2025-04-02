#include "medOpCreateLabeledVolume.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEVolumeGray.h"

#include "medVMELabeledVolume.h"

//----------------------------------------------------------------------------
medOpCreateLabeledVolume::medOpCreateLabeledVolume(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
medOpCreateLabeledVolume::~medOpCreateLabeledVolume( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* medOpCreateLabeledVolume::Copy()   
//----------------------------------------------------------------------------
{
	return new medOpCreateLabeledVolume(GetLabel());
}
//----------------------------------------------------------------------------
bool medOpCreateLabeledVolume::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMEVolumeGray));
}
//----------------------------------------------------------------------------
void medOpCreateLabeledVolume::OpRun()   
//----------------------------------------------------------------------------
{
  auto labeledVolume = medVMELabeledVolume::NewSPtr();
  labeledVolume->SetName(_R("Labeled Volume"));
  SetOutput(labeledVolume);
  labeledVolume->SetVolumeLink(GetInput().get());
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
