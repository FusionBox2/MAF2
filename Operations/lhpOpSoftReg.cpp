/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpSoftReg.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-14 12:03:18 $
  Version:   $Revision: 1.5 $
  Authors:   Fedor Moiseev / Vladik Aranov
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

#include "lhpOpSoftReg.h"

#include "wx/textfile.h"
#include "wx/arrimpl.cpp"
#include <wx/wxprec.h>
#include "wx/busyinfo.h"
#include <math.h>

#include "mafDecl.h"
#include "mafTagArray.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafOpExplodeCollapse.h"

#include "mafSmartPointer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafVMELandmark.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"
#include "mafTransformBase.h"

#include "vtkMAFSmartPointer.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

#include "mafVectors.h"

#define SURF_VERSION

//----------------------------------------------------------------------------
// Required for MSVC
//----------------------------------------------------------------------------
#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------


bool lhpOpSoftReg::BonesSetAccept(mafNode* vme)
{
  if(vme == NULL)
    return false;

  for(int i=0; i < vme->GetNumberOfChildren(); i++)
  {
    mafVME *childVme = mafVME::SafeDownCast(vme->GetChild(i));
    if(childVme == NULL)
      continue;
    if(mafVMEOutputSurface::SafeDownCast(childVme->GetOutput()) != NULL)
      return true;
  }

  return false;
}
mafCxxTypeMacro(lhpOpSoftReg)

//----------------------------------------------------------------------------
lhpOpSoftReg::lhpOpSoftReg(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType    = OPTYPE_OP;
  m_Canundo   = true;
  m_Bones     = NULL;
  m_BonesName = "none";
  m_Sample    = 0.0;

}

//----------------------------------------------------------------------------
lhpOpSoftReg::~lhpOpSoftReg()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
mafOp* lhpOpSoftReg::Copy()
//----------------------------------------------------------------------------
{
  return new lhpOpSoftReg(GetLabel());
}

//----------------------------------------------------------------------------
bool lhpOpSoftReg::Accept(mafNode* vme)
//----------------------------------------------------------------------------
#ifndef SURF_VERSION
{
  if(vme == NULL)
    return false;

  if(vme->IsMAFType(mafVMELandmarkCloud))
    return true;

  for(int i=0; i < vme->GetNumberOfChildren(); i++)
  {
    if(vme->GetChild(i)->IsMAFType(mafVMELandmarkCloud))
      return true;
  }

  return false;
}
#else
{
  if(vme == NULL)
    return false;
  if(mafVME::SafeDownCast(vme)== NULL)
    return false;
  return mafVME::SafeDownCast(vme)->GetOutput()->IsMAFType(mafVMEOutputSurface);
}
#endif

//----------------------------------------------------------------------------
void lhpOpSoftReg::OpRun()
//----------------------------------------------------------------------------
{
  if(m_Gui == NULL)
  {
    m_Gui = new mafGUI(this);
    m_Gui->Label(_("bones :"),true);
    m_Gui->Label(&m_BonesName);

    m_Gui->Button(ID_SELECTBONES, _("select bones"));
    m_Gui->Label("");
    m_Gui->OkCancel();
    m_Gui->Enable(wxOK,false);
  }
  ShowGui();
}

//----------------------------------------------------------------------------
void lhpOpSoftReg::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
  {
    case wxOK:          
      OpStop(OP_RUN_OK);
      break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);
      break;
    case ID_SELECTBONES:
      {
        mafString s(_("Choose cloud"));
        mafEvent e(this,VME_CHOOSE, &s, (long)&lhpOpSoftReg::BonesSetAccept);
        mafEventMacro(e);
        mafNode *vme = e.GetVme();
        OnChooseVme(vme);
        break;
      }
    default:
      mafEventMacro(*maf_event);
    break;
  }
}

double CalcDistance(mafVMEOutputSurface *out_surface, mafTimeStamp ts, const V3d<double>& point, V3d<double> &locPnt)
{
  vtkPoints    *pts;
  vtkCellArray *polys;
  vtkPolyData  *input = out_surface->GetSurfaceData();
  polys = input->GetPolys();
  pts   = input->GetPoints();

  mafMatrix transf;
  out_surface->GetAbsMatrix(transf, ts);

  V4d<double> inpnt(point.x, point.y, point.z, 1.0);
  V4d<double> otpnt;

  transf.Invert();
  transf.MultiplyPoint(inpnt.components, otpnt.components);

  V3d<double> res(otpnt.components);
  locPnt = res;

  vtkIdType npnts = pts->GetNumberOfPoints();
  vtkIdType nclls = polys->GetNumberOfCells();

  std::vector<V3d<double> > coords;
  coords.resize(npnts);
  for(unsigned i = 0; i < npnts; i++)
  {
    pts->GetPoint(i, coords[i].components);
  }
  double dist = res | coords[0];
  for(unsigned i = 1; i < npnts; i++)
  {
    double d = res | coords[i];
    if(d < dist)
      dist = d;
  }
  return sqrt(dist);
}


//----------------------------------------------------------------------------
void lhpOpSoftReg::OpDo()
//----------------------------------------------------------------------------
#ifndef SURF_VERSION
{
  for(unsigned i = 0; i < m_Input->GetNumberOfChildren(); i++)
  {
    mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Input->GetChild(i));
    mafMatrix cloudAbs;
    if(cloud == NULL)
      continue;
    V4d<double> invec;
    V4d<double> outvec;
    cloud->GetOutput()->GetAbsMatrix(cloudAbs);

    for(unsigned j = 0; j < cloud->GetNumberOfLandmarks(); j++)
    {
      std::vector<mafTimeStamp> frames;
      cloud->GetLandmark(j, invec.components);
      invec.w = 1.0;
      cloudAbs.MultiplyPoint(invec.components, outvec.components);
      V3d<double> lmPos(outvec.components);
      V3d<double> lmPosBone;
      std::vector<V3d<double> > lmPoses;
      std::vector<double >      dists;
      double sumdist = 0.0;
      for(unsigned k = 0; k < m_Bones->GetNumberOfChildren(); k++)
      {
        mafVME *childVme = mafVME::SafeDownCast(m_Bones->GetChild(k));
        if(childVme == NULL)
          continue;

        mafVMEOutputSurface *out_surface = mafVMEOutputSurface::SafeDownCast(childVme->GetOutput());
        if(out_surface == NULL)
          continue;
        out_surface->Update();
        double dst = CalcDistance(out_surface, m_Sample, lmPos, lmPosBone);
        sumdist += 100.0 / dst;
        dists.push_back(100.0 / dst);
        lmPoses.push_back(lmPosBone);
        childVme->GetTimeStamps(frames);
      }

      for(unsigned l = 0; l < frames.size(); l++)
      {
        unsigned aa = 0;
        V3d<double> lmCurTS(0.0, 0.0, 0.0);
        for(unsigned k = 0; k < m_Bones->GetNumberOfChildren(); k++)
        {
          mafVME *childVme = mafVME::SafeDownCast(m_Bones->GetChild(k));
          if(childVme == NULL)
            continue;
          mafVMEOutputSurface *out_surface = mafVMEOutputSurface::SafeDownCast(childVme->GetOutput());
          if(out_surface == NULL)
            continue;
          mafMatrix transf;
          out_surface->GetAbsMatrix(transf, frames[l]);
          V4d<double> pntsrc(lmPoses[aa].x, lmPoses[aa].y, lmPoses[aa].z, 1.0);
          V4d<double> pnttrg;
          transf.MultiplyPoint(pntsrc.components, pnttrg.components);
          V3d<double> fin(pnttrg.components);
          lmCurTS += fin * (dists[aa] / sumdist);
          aa++;
          cloud->SetLandmark(j, lmCurTS.x, lmCurTS.y, lmCurTS.z, frames[l]);
        }
      }
    }
  }
  return;
}
#else
{
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
  coords.resize(npnts);
  for(unsigned i = 0; i < npnts; i++)
  {
    pts->GetPoint(i, coords[i].components);
  }

  vtkPoints    *newPts;
  vtkCellArray *newPolys;
  vtkPolyData  *output;


  std::vector<mafTimeStamp> frames;
  for(unsigned k = 0; k < m_Bones->GetNumberOfChildren(); k++)
  {
    mafVME *childVme = mafVME::SafeDownCast(m_Bones->GetChild(k));
    if(childVme == NULL)
      continue;
    childVme->GetTimeStamps(frames);
  }

  for(unsigned l = 0; l < frames.size(); l++)
  {
    vtkNEW(output);

    // All of the data in the first piece.
    if(output->GetUpdatePiece() > 0)
    {
      return;
    }

    newPts = vtkPoints::New();
    newPts->Allocate(npnts,10000);
    newPolys = vtkCellArray::New();
    newPolys->Allocate(nclls, 20000);

    double x[3];

    for(unsigned i = 0; i < npnts; i++)
    {
      newPts->InsertNextPoint(x);
    }



    for(unsigned j = 0; j < npnts; j++)
    {
      V3d<double> lmPos;
      V3d<double> lmPosBone;
      pts->GetPoint(j, lmPos.components);
      std::vector<V3d<double> > lmPoses;
      std::vector<double >      dists;
      double sumdist = 0.0;
      for(unsigned k = 0; k < m_Bones->GetNumberOfChildren(); k++)
      {
        mafVME *childVme = mafVME::SafeDownCast(m_Bones->GetChild(k));
        if(childVme == NULL)
          continue;

        mafVMEOutputSurface *out_surface = mafVMEOutputSurface::SafeDownCast(childVme->GetOutput());
        if(out_surface == NULL)
          continue;
        out_surface->Update();
        double dst = CalcDistance(out_surface, m_Sample, lmPos, lmPosBone);
        sumdist += 100.0 / dst;
        dists.push_back(100.0 / dst);
        lmPoses.push_back(lmPosBone);
      }

      unsigned aa = 0;
      V3d<double> lmCurTS(0.0, 0.0, 0.0);
      for(unsigned k = 0; k < m_Bones->GetNumberOfChildren(); k++)
      {
        mafVME *childVme = mafVME::SafeDownCast(m_Bones->GetChild(k));
        if(childVme == NULL)
          continue;
        mafVMEOutputSurface *out_surface = mafVMEOutputSurface::SafeDownCast(childVme->GetOutput());
        if(out_surface == NULL)
          continue;
        mafMatrix transf;
        out_surface->GetAbsMatrix(transf, frames[l]);
        V4d<double> pntsrc(lmPoses[aa].x, lmPoses[aa].y, lmPoses[aa].z, 1.0);
        V4d<double> pnttrg;
        transf.MultiplyPoint(pntsrc.components, pnttrg.components);
        V3d<double> fin(pnttrg.components);
        lmCurTS += fin * (dists[aa] / sumdist);
        aa++;
        //coords[j] = lmCurTS;
      }
      coords[j] = lmCurTS;
    }





    for(unsigned i = 0; i < npnts; i++)
    {
      newPts->SetPoint(i, coords[i][0], coords[i][1], coords[i][2]);
    }

    {
      int counter = 0;
      vtkIdType npts = 0;
      vtkIdType *indx = 0;

      for (polys->InitTraversal(); polys->GetNextCell(npts, indx); )
      {
        newPolys->InsertNextCell(npts, indx);
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
    mafVMESurface *outSurface;
    mafNEW(outSurface);
    outSurface->SetName("cutted");
    outSurface->SetData(output,t);

    mafTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("NATURAL");

    outSurface->GetTagArray()->SetTag(tag_Nature);

    vtkDEL(output);

    if(outSurface)
    {
      outSurface->ReparentTo(m_Input->GetRoot());
      mafEventMacro(mafEvent(this, VME_ADD, outSurface));
    }
    mafDEL(outSurface);

  }


  mafEventMacro(mafEvent(this,CAMERA_UPDATE));


}
#endif



//----------------------------------------------------------------------------
void lhpOpSoftReg::OpUndo()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void lhpOpSoftReg::OnChooseVme(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(!vme) // user choose cancel - keep everything as before
    return;

  m_Bones = mafVME::SafeDownCast(vme);
  if(!m_Bones)
    return;
  m_BonesName = m_Bones->GetName();
  m_Gui->Enable(wxOK,true);
  m_Gui->Update();
}