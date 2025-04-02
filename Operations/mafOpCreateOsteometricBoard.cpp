#include "mafOpCreateOsteometricBoard.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEOsteometricBoard.h"

//----------------------------------------------------------------------------
mafOpCreateOsteometricBoard::mafOpCreateOsteometricBoard(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateOsteometricBoard::~mafOpCreateOsteometricBoard()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateOsteometricBoard::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateOsteometricBoard(GetLabel());
}

//----------------------------------------------------------------------------
bool mafOpCreateOsteometricBoard::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}

//----------------------------------------------------------------------------
void mafOpCreateOsteometricBoard::OpRun()
//----------------------------------------------------------------------------
{
	auto osteometricBoard = mafVMEOsteometricBoard::NewSPtr();

	osteometricBoard->SetName(_R("OsteometricBoard"));
	SetOutput(osteometricBoard);
	{mafEvent evUnq(this, OP_RUN_OK); InvokeEvent(evUnq);}
}
