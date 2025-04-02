#include "mafOpCreateSpline.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEPolylineSpline.h"

//----------------------------------------------------------------------------
mafOpCreateSpline::mafOpCreateSpline(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateSpline::~mafOpCreateSpline( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateSpline::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateSpline(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateSpline::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateSpline::OpRun()   
//----------------------------------------------------------------------------
{
  auto polylineSpline = mafVMEPolylineSpline::NewSPtr();
  polylineSpline->SetName(_R("Polyline Spline"));
  SetOutput(polylineSpline);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
