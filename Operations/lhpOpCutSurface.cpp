/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpCutSurface.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-19 11:42:56 $
  Version:   $Revision: 1.3 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpOpCutSurface.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"

#include "mafDecl.h"
#include "mafTagArray.h"
#include "mafGUI.h"
#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEOutputSurface.h"
#include "mafTransformBase.h"

#include "vtkMAFSmartPointer.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

#include "mafVectors.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpCutSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
 lhpOpCutSurface:: lhpOpCutSurface(const wxString &label) : mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType     = OPTYPE_OP;
  m_Canundo    = true;
  m_OutSurface = NULL;
}
//----------------------------------------------------------------------------
 lhpOpCutSurface::~lhpOpCutSurface()
//----------------------------------------------------------------------------
{
  mafDEL(m_OutSurface);
}  

//----------------------------------------------------------------------------
bool lhpOpCutSurface::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(vme == NULL)
    return false;
  if(mafVME::SafeDownCast(vme)== NULL)
    return false;
  return mafVME::SafeDownCast(vme)->GetOutput()->IsMAFType(mafVMEOutputSurface);
}

//----------------------------------------------------------------------------
mafOp * lhpOpCutSurface::Copy()
//----------------------------------------------------------------------------
{
  return new lhpOpCutSurface(m_Label);
}


//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
  ID_DEFAULT = MINID,
  ID_PARAMSURF,
  ID_LAST,
  ID_FORCED_DWORD = 0x7fffffff
};


//----------------------------------------------------------------------------
void lhpOpCutSurface::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);
  m_Gui->Button(ID_PARAMSURF, "Cutting surf", "", "Press to select." );  
  m_Gui->Label("Wnd d.VME",&m_CutSurfName);
  m_Gui->OkCancel();
  ShowGui();
}

//----------------------------------------------------------------------------
void lhpOpCutSurface::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
  {
  case wxOK:          
    { 
      OpStop(OP_RUN_OK);
    }
    break;
  case wxCANCEL:
    {    
      OpStop(OP_RUN_CANCEL);
    }
    break;
  case ID_PARAMSURF:
    {
      mafString s("Choose cutting surface");
      mafEvent e(this,VME_CHOOSE, &s);
      mafEventMacro(e);
      if(e.GetVme() == NULL)
      {
        return;
      }
      if(!e.GetVme()->IsA("mafVMESurfaceParametric"))
      {
        wxMessageBox("Selected VME should be of mafVMESurfaceParametric type.","Warning", wxOK|wxICON_WARNING , NULL);
        return;
      }

      mafVMESurfaceParametric* sf = (mafVMESurfaceParametric*)e.GetVme();
      if(sf->GetGeometryType() != mafVMESurfaceParametric::PARAMETRIC_PLANE)
      {
        wxMessageBox("Selected surface should be of PLANE type.","Warning", wxOK|wxICON_WARNING , NULL);
        return;
      }
      m_CutSurf = sf;
      SetNodeName(m_CutSurf, &m_CutSurfName);
      m_Gui->Update();
    }
    break;
  default:
    {
      mafEventMacro(*maf_event); 
    }
    break;
  }
}

bool passFilter(V3d<double>& x, V3d<double>& norm, double d)
{
  return (x * norm <= d);
}

struct _edge
{
  vtkIdType v1, v2;
  bool      inverse;
};
//----------------------------------------------------------------------------
void lhpOpCutSurface::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    HideGui();
  }
  HideGui();

  mafVMEOutputSurface *out_surface = mafVMEOutputSurface::SafeDownCast(mafVME::SafeDownCast(m_Input)->GetOutput());
  out_surface->Update();

  vtkMAFSmartPointer<vtkTriangleFilter>triangles;
  vtkMAFSmartPointer<vtkTransformPolyDataFilter> v_tpdf;
  triangles->SetInput(out_surface->GetSurfaceData());
  triangles->Update();

  v_tpdf->SetInput(triangles->GetOutput());
  v_tpdf->SetTransform(out_surface->GetAbsTransform()->GetVTKTransform());
  v_tpdf->Update();


  vtkPoints    *pts;
  vtkCellArray *polys;
  vtkPolyData  *input = v_tpdf->GetOutput();
  polys = input->GetPolys();
  pts   = input->GetPoints();

  vtkIdType npnts = pts->GetNumberOfPoints();
  vtkIdType nclls = polys->GetNumberOfCells();

  std::vector<V3d<double> > coords;
  std::vector<bool> rest;
  std::vector<vtkIdType> reIndex;
  std::vector<bool> restPoly;
  std::vector<_edge> edges;
  coords.resize(npnts);
  rest.resize(npnts);
  reIndex.resize(npnts);
  restPoly.resize(nclls);
  for(unsigned i = 0; i < nclls; i++)
  {
    restPoly[i] = false;
  }
  for(unsigned i = 0; i < npnts; i++)
  {
    pts->GetPoint(i, coords[i].components);
    rest[i] = false;
    reIndex[i] = i;
  }


  /*V3d<double> orig(m_CutSurf->GetPlaneOrigin());
  V3d<double> pnt1(m_CutSurf->GetPlanePoint1());
  V3d<double> pnt2(m_CutSurf->GetPlanePoint2());

  V3d<double> diff1 = pnt1 - orig;
  V3d<double> diff2 = pnt2 - orig;

  V3d<double> norml = diff1 ^ diff2;
  double      dplane = orig * norml;*/
  unsigned    numPolysFiltered = 0;
  {
    int counter = 0;
    vtkIdType npts = 0;
    vtkIdType *indx = 0;
    unsigned plInd = 0;

    for (polys->InitTraversal(); polys->GetNextCell(npts, indx); )
    {
      bool passed = false;
      for(unsigned jj = 0; jj < npts; jj++)
      {
        //if(passFilter(coords[indx[jj]], norml, dplane))
        {
          passed = true;
          break;
        }
      }
      if(passed)
      {
        for(unsigned jj = 0; jj < npts; jj++)
          rest[indx[jj]] = true;
        numPolysFiltered++;
        restPoly[plInd] = true;
      }
      plInd++;
    }
  }


  unsigned numVertsFiltered = 0;
  for(unsigned i = 0; i < npnts; i++)
  {
    if(rest[i])
    {
      reIndex[i] = numVertsFiltered++;
    }
  }

  vtkPoints    *newPts;
  vtkCellArray *newPolys;
  vtkPolyData  *output;

  vtkNEW(output);

  // All of the data in the first piece.
  if(output->GetUpdatePiece() > 0)
  {
    return;
  }

  newPts = vtkPoints::New();
  newPts->Allocate(numVertsFiltered,10000);
  newPolys = vtkCellArray::New();
  newPolys->Allocate(numPolysFiltered, 20000);

  double x[3];

  for(unsigned i = 0; i < numVertsFiltered; i++)
  {
    newPts->InsertNextPoint(x);
  }
  for(unsigned i = 0; i < npnts; i++)
  {
    if(rest[i])
    {
      newPts->SetPoint(reIndex[i], coords[i][0], coords[i][1], coords[i][2]);
    }
  }

  {
    int counter = 0;
    vtkIdType npts = 0;
    vtkIdType *indx = 0;
    unsigned plInd = 0;
    std::vector<vtkIdType> polyreind;

    for (polys->InitTraversal(); polys->GetNextCell(npts, indx); )
    {
      if(restPoly[plInd])
      {
        polyreind.resize(npts);
        for(unsigned jj = 0; jj < npts; jj++)
        {
          polyreind[jj] = reIndex[indx[jj]];
        }
        newPolys->InsertNextCell(npts, &polyreind[0]);
      }
      plInd++;
    }
  }

  //
  // Update ourselves
  //
  output->SetPoints(newPts);
  output->SetPolys(newPolys);
  output->Squeeze();
  newPts->Delete();
  newPolys->Delete();

  mafTimeStamp t;
  t = ((mafVME *)m_Input)->GetTimeStamp();
  mafNEW(m_OutSurface);
  m_OutSurface->SetName("cutted");
  m_OutSurface->SetData(output,t);

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  m_OutSurface->GetTagArray()->SetTag(tag_Nature);

  vtkDEL(output);

  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void lhpOpCutSurface::SetNodeName(mafVME *pVME, mafString *pName) 
//----------------------------------------------------------------------------
{
  *pName = pVME->GetName();
  if(pVME->GetParent() != NULL)
  {
    *pName = *pName + " parent:";
    *pName = *pName + pVME->GetParent()->GetName();
  }
}




//----------------------------------------------------------------------------
void  lhpOpCutSurface::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
}


//----------------------------------------------------------------------------
void lhpOpCutSurface::OpDo()   
//----------------------------------------------------------------------------
{
  if(m_OutSurface)
  {
    m_OutSurface->ReparentTo(m_Input->GetRoot());
    mafEventMacro(mafEvent(this, VME_ADD, m_OutSurface));
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void lhpOpCutSurface::OpUndo()   
//----------------------------------------------------------------------------
{
  if(m_OutSurface)
  {
    mafEventMacro(mafEvent(this, VME_REMOVE, m_OutSurface));
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
