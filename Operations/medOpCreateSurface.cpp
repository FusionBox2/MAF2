#include "medOpCreateSurface.h"

#include "mafGUI.h"

#include "mafNode.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMESurface.h"

#include "vtkPolyData.h"

//----------------------------------------------------------------------------
medOpCreateSurface::medOpCreateSurface(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = true;
	m_InputPreserving = true;

}
//----------------------------------------------------------------------------
medOpCreateSurface::~medOpCreateSurface()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* medOpCreateSurface::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new medOpCreateSurface(GetLabel());
}
//----------------------------------------------------------------------------
bool medOpCreateSurface::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	return vme != NULL && vme->IsMAFType(mafVMESurfaceParametric);
}
//----------------------------------------------------------------------------
void medOpCreateSurface::OpRun()
//----------------------------------------------------------------------------
{
	auto inputSurface=mafVMESurfaceParametric::SafeDownCast(GetInput());
	
	auto surface = mafVMESurface::NewSPtr();
	surface->SetName(inputSurface->GetName());
	surface->SetData(vtkPolyData::SafeDownCast(inputSurface->GetOutput()->GetVTKData()),inputSurface->GetTimeStamp());
	surface->Update();

	SetOutput(surface);
	
	OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void medOpCreateSurface::OpStop(int result)   
//----------------------------------------------------------------------------
{
	{mafEvent evUnq(this,result); InvokeEvent(evUnq);}
}