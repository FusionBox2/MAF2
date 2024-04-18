/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpJoinSurf.cpp,v $
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

#include "lhpOpJoinSurf.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"

#include "mafDecl.h"
#include "mafTagArray.h"
#include "mafGUI.h"
#include "mafVMESurface.h"
#include "mafVMEOutputSurface.h"
#include "mafTransformBase.h"

#include "vtkSmartPointer.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

#include "mafVectors.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpJoinSurf);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
 lhpOpJoinSurf:: lhpOpJoinSurf(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType       = OPTYPE_OP;
  m_Canundo      = true;
  m_OutSurface   = NULL;
  m_JoinSurf.clear();
}
//----------------------------------------------------------------------------
 lhpOpJoinSurf::~lhpOpJoinSurf()
//----------------------------------------------------------------------------
{
  mafDEL(m_OutSurface);
}  

//----------------------------------------------------------------------------
bool lhpOpJoinSurf::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(vme == NULL)
    return false;
  if(mafVME::SafeDownCast(vme)== NULL)
    return false;
  return mafVME::SafeDownCast(vme)->GetOutput()->IsMAFType(mafVMEOutputSurface);
}

//----------------------------------------------------------------------------
mafOp * lhpOpJoinSurf::Copy()
//----------------------------------------------------------------------------
{
  lhpOpJoinSurf *cp = new  lhpOpJoinSurf(GetLabel());
  return cp; 
}


//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void lhpOpJoinSurf::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);
  m_Gui->Label(_L("Surfaces"), true);
  m_ListBox = m_Gui->ListBox(ID_JOINSURF/*LISTBOX*/);

  for(int j = 0; j < m_JoinSurf.size();j++)
  {
    m_ListBox->Append(m_JoinSurf[j]->GetName().toWx());
  }


  m_Gui->Button(ID_ADD_SURF, _L("Add"), _R(""), _R(""));
  m_Gui->Button(ID_REMOVE_SURF, _L("Remove"), _R(""), _R(""));

  m_Gui->OkCancel();
  ShowGui();
}

//----------------------------------------------------------------------------
void lhpOpJoinSurf::OnEvent(mafEventBase *maf_event) 
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
  case ID_ADD_SURF:
    {
      /*if(m_ListBox->GetCount()!=0)
      {
      wxMessageBox("Current max point number is one!");
      return;
      }*/
      mafString s(_R("Choose surface to join"));
      mafEvent e(this,VME_CHOOSE, &s);
      mafEventMacro(e);
      if(e.GetVme() == NULL)
      {
        return;
      }
      mafNode *sel = e.GetVme();
      if(mafVME::SafeDownCast(sel)== NULL || !mafVME::SafeDownCast(sel)->GetOutput()->IsMAFType(mafVMEOutputSurface))
      {
        mafWarningMessage(_M("Selected VME should have mafVMEOutputSurface as Output."));
        return;
      }
      if(sel == m_Input)
      {
        mafWarningMessage(_M("Selected VME should not be the same as Input."));
        return;
      }

      mafString t;
      t = mafVME::SafeDownCast(sel)->GetName();
      m_ListBox->Append(_(t.toWx()));
      m_ListBox->SetStringSelection(_(t.toWx()));
      m_JoinSurf.push_back(mafVME::SafeDownCast(sel));


      m_ListBox->Update();
      m_Gui->Update();
    }
    break;
  case ID_REMOVE_SURF:
    { 
      if(m_ListBox->GetCount()!=0)
      {
        int number = m_ListBox->GetSelection();
        if(number != -1)
        {
          m_ListBox->Delete(number);
          for(int i = number + 1; i < m_JoinSurf.size();i++)
          {
            m_JoinSurf[i - 1] = m_JoinSurf[i];
          }
          m_JoinSurf.resize(m_JoinSurf.size());
          m_Gui->Update();
        }

      }
    }
    break;

  /*case ID_JOINSURF:
    {
      mafString s("Choose surface to join");
      mafEvent e(this,VME_CHOOSE, &s);
      mafEventMacro(e);
      if(e.GetVme() == NULL)
      {
        return;
      }
      mafNode *sel = e.GetVme();
      if(mafVME::SafeDownCast(sel)== NULL || !mafVME::SafeDownCast(sel)->GetOutput()->IsMAFType(mafVMEOutputSurface))
      {
        wxMessageBox("Selected VME should have mafVMEOutputSurface as Output.","Warning", wxOK|wxICON_WARNING , NULL);
        return;
      }

      m_JoinSurf = mafVME::SafeDownCast(sel);
      SetNodeName(m_JoinSurf, &m_JoinSurfName);
      m_Gui->Update();
    }
    break;*/
  default:
    {
      mafEventMacro(*maf_event); 
    }
    break;
  }
}


//struct _edge
//{
//  vtkIdType v1, v2;
//  bool      inverse;
//};
//----------------------------------------------------------------------------
void lhpOpJoinSurf::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  if (result == OP_RUN_CANCEL)
  {
    mafEventMacro(mafEvent(this,result));
    return;
  }

  mafVMEOutputSurface *surface1 = mafVMEOutputSurface::SafeDownCast(mafVME::SafeDownCast(m_Input)->GetOutput());
  surface1->Update();

  int totalVerts = surface1->GetSurfaceData()->GetNumberOfPoints();
  int totalCells = surface1->GetSurfaceData()->GetNumberOfCells();

  vtkNew<vtkTriangleFilter> triangles1;
  vtkNew<vtkTransformPolyDataFilter> v_tpdf1;
  triangles1->SetInputConnection(surface1->GetVTKOutputPort());
  triangles1->Update();

  v_tpdf1->SetInputConnection(triangles1->GetOutputPort());
  v_tpdf1->SetTransform(surface1->GetAbsTransform()->GetVTKTransform());
  v_tpdf1->Update();


  vtkPoints    *pts1;
  vtkCellArray *polys1;
  vtkPolyData  *input1 = v_tpdf1->GetOutput();
  polys1 = input1->GetPolys();
  pts1   = input1->GetPoints();

  vtkIdType npnts1 = pts1->GetNumberOfPoints();
  vtkIdType nclls1 = polys1->GetNumberOfCells();

  for(unsigned i = 0; i < m_JoinSurf.size(); i++)
  {
    mafVMEOutputSurface *surf = mafVMEOutputSurface::SafeDownCast(mafVME::SafeDownCast(m_JoinSurf[i])->GetOutput());
    surf->Update();

    totalVerts += surf->GetSurfaceData()->GetNumberOfPoints();
    totalCells += surf->GetSurfaceData()->GetNumberOfCells();
  }


  vtkPoints    *newPts;
  vtkCellArray *newPolys;
  vtkPolyData  *output;

  vtkNEW(output);

  // All of the data in the first piece.
  //if(output->GetUpdatePiece() > 0)
  {
  //  return;
  }

  newPts = vtkPoints::New();
  newPts->Allocate(totalVerts,10000);
  newPolys = vtkCellArray::New();
  newPolys->Allocate(totalCells, 20000);

  V3d<double> coord;
  for(unsigned i = 0; i < npnts1; i++)
  {
    pts1->GetPoint(i, coord.components);
    newPts->InsertNextPoint(coord.components);
  }

  vtkIdType npts = 0;
#if VTK_MAJOR_VERSION > 8
  const vtkIdType* indx = 0;
#else
  vtkIdType* indx = 0;
#endif

  for (polys1->InitTraversal(); polys1->GetNextCell(npts, indx); )
  {
    newPolys->InsertNextCell(npts, indx);
  }

  int vertsInserted = npnts1;
  for(unsigned k = 0; k < m_JoinSurf.size(); k++)
  {
    mafVMEOutputSurface *surface2 = mafVMEOutputSurface::SafeDownCast(m_JoinSurf[k]->GetOutput());
    surface2->Update();

    vtkNew<vtkTriangleFilter> triangles2;
    vtkNew<vtkTransformPolyDataFilter> v_tpdf2;
    triangles2->SetInputConnection(surface2->GetVTKOutputPort());
    triangles2->Update();

    v_tpdf2->SetInputConnection(triangles2->GetOutputPort());
    v_tpdf2->SetTransform(surface2->GetAbsTransform()->GetVTKTransform());
    v_tpdf2->Update();


    vtkPoints    *pts2;
    vtkCellArray *polys2;
    vtkPolyData  *input2 = v_tpdf2->GetOutput();
    polys2 = input2->GetPolys();
    pts2   = input2->GetPoints();

    vtkIdType npnts2 = pts2->GetNumberOfPoints();
    vtkIdType nclls2 = polys2->GetNumberOfCells();

    for(unsigned i = 0; i < npnts2; i++)
    {
      pts2->GetPoint(i, coord.components);
      newPts->InsertNextPoint(coord.components);
    }
    for (polys2->InitTraversal(); polys2->GetNextCell(npts, indx); )
    {
      std::vector<vtkIdType> indxs2(npts);
      for(unsigned i = 0; i < npts; i++)
        indxs2[i] = indx[i] + vertsInserted;
      newPolys->InsertNextCell(npts, &indxs2[0]);
    }
    vertsInserted += npnts2;
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
  mafString nm;
  nm = m_Input->GetName();
  nm += _R("_merged");
  //nm += m_JoinSurf[0]->GetName();
  m_OutSurface->SetName(nm);
  m_OutSurface->SetData(output,t);

  mafTagItem tag_Nature;
  tag_Nature.SetName(_R("VME_NATURE"));
  tag_Nature.SetValue(_R("NATURAL"));

  m_OutSurface->GetTagArray()->SetTag(tag_Nature);

  vtkDEL(output);

  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void lhpOpJoinSurf::SetNodeName(mafVME *pVME, mafString *pName) 
//----------------------------------------------------------------------------
{
  *pName = pVME->GetName();
  if(pVME->GetParent() != NULL)
  {
    *pName += _R(" parent:");
    *pName += pVME->GetParent()->GetName();
  }
}




//----------------------------------------------------------------------------
void  lhpOpJoinSurf::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
}


//----------------------------------------------------------------------------
void lhpOpJoinSurf::OpDo()   
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
void lhpOpJoinSurf::OpUndo()   
//----------------------------------------------------------------------------
{
  if(m_OutSurface)
  {
    mafEventMacro(mafEvent(this, VME_REMOVE, m_OutSurface));
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
