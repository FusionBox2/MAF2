/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpFindCentroid.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "lhpOpFindCentroid.h"
#include "mafDecl.h"

#include "mafTagArray.h"
#include "mafVectors.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"
#include "vtkMAFSmartPointer.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEOutputSurface.h"
#include "mafTransformBase.h"
#include "mafDataVector.h"
#include "mafVMEItemVTK.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpFindCentroid);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpFindCentroid::lhpOpFindCentroid(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
lhpOpFindCentroid::~lhpOpFindCentroid( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Output);
}
//----------------------------------------------------------------------------
mafOp* lhpOpFindCentroid::Copy()   
//----------------------------------------------------------------------------
{
	return new lhpOpFindCentroid(GetLabel());
}
//----------------------------------------------------------------------------
bool lhpOpFindCentroid::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  if(node == NULL)
    return false;
  if(mafVMEGenericAbstract::SafeDownCast(node)== NULL)
    return false;
  return mafVME::SafeDownCast(node)->GetOutput()->IsMAFType(mafVMEOutputSurface);
}
//----------------------------------------------------------------------------
void lhpOpFindCentroid::OpRun()
//----------------------------------------------------------------------------
{
  mafVMEGenericAbstract *gabs = mafVMEGenericAbstract::SafeDownCast(m_Input);
  if(!gabs)
    mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
  mafDataVector * dvect = gabs->GetDataVector();
  if(!dvect)
    mafEventMacro(mafEvent(this,OP_RUN_CANCEL));

  mafVMELandmarkCloud *cloud = NULL;

  for(mafDataVector::Iterator it = dvect->Begin(); it != dvect->End(); ++it)
  {
    mafVMEItemVTK *ivtk = mafVMEItemVTK::SafeDownCast(it->second);
    if(!ivtk)
      continue;
    vtkPoints    *pts;
    vtkPolyData  *input = (vtkPolyData*)ivtk->GetData();
    pts   = input->GetPoints();
    V3d<double> centroid;

    vtkIdType npnts = pts->GetNumberOfPoints();
    for(unsigned i = 0; i < npnts; i++)
    {
      V3d<double> coord;
      pts->GetPoint(i, coord.components);
      centroid += coord;
    }
    if(npnts > 0)
      centroid /= npnts;
    if(!cloud)
    {
      mafNEW(cloud);
      if(!cloud)
        mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
      cloud->SetName("Centroid");
      mafTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");
      cloud->GetTagArray()->SetTag(tag_Nature);
      cloud->AppendLandmark("Centroid");
    }
    cloud->SetLandmark(0, centroid.x, centroid.y, centroid.z, it->first);
  }
  if(!cloud)
    mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
  m_Output = cloud;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
