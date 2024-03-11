/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpLMProj.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:14:48 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani - porting  Daniele Giunchi
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

#include "lhpOpLMProj.h"
#include <wx/busyinfo.h>

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"


#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"

#include "vtkPolyData.h"
#include "vtkMEDPolyDataMirror.h"
#include "mafVectors.h"
#include <vector>


#include "mafVMESurface.h"
#include "mafVMEOutputSurface.h"
#include "mafTransformBase.h"

#include "vtkMAFSmartPointer.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpLMProj);
//----------------------------------------------------------------------------

namespace
{

  V3d<double> edgevertex(const V3d<double>& s, const V3d<double>& t, const V3d<double>& v)
  {
    V3d<double> sub, e;
    double      coef;
    sub = v - s;
    e   = t - s;
    double ln = e.length2();
    if(ln != 0.0) 
      e /= sqrt(ln);
    coef = sub * e;
    if(coef > 1.0)
      coef = 1.0;
    if(coef < 0.0)
      coef = 0.0;
    return s + e * coef;
  }



  bool facevertex(std::vector<V3d<double> >& verts, V3d<double>& v, V3d<double>& n)
  {
    std::vector<V3d<double> > crosses;
    std::vector<V3d<double> > toPt;
    std::vector<V3d<double> > sides;
    std::vector<V3d<double> > nearPoints;
    unsigned                  i, nMin;
    double                    dist, curDist, planepos;
    bool                      bRes;
    V3d<double>               nearest, normal;

    crosses.resize(verts.size());
    sides.resize(verts.size());
    toPt.resize(verts.size());
    nearPoints.resize(verts.size());
    //calculate projection of point to face plane
    //DiV4dLineComb(&fpV->lvpVertex->vCoords, 1.f, &fpF->lfpFace->vNormal, -SaMathFullDotProduct(&fpV->lvpVertex->vCoords, &fpF->lfpFace->vNormal), &vNear);

    //check if this point is internal point of polygon
    for(i = 0; i < verts.size(); i++)
      sides[i] = verts[(i + 1) % verts.size()] - verts[i];
    normal[0] = 0.0;
    normal[1] = 0.0;
    normal[2] = 0.0;
    for(i = 0; i < verts.size(); i++)
      normal += sides[i] ^ sides[(i + 1) % verts.size()];
    double ln = normal.length2();
    if(ln != 0.0) 
      normal /= sqrt(ln);
    planepos = -(normal * verts[0]);
    nearest = v - (v * normal + planepos) * normal;


    for(i = 0; i < verts.size(); i++)
      toPt[i] = nearest - verts[i];
    for(i = 0; i < verts.size(); i++)
      crosses[i] = sides[i] ^ toPt[(i + 1) % verts.size()];
    bRes = TRUE;
    for(i = 0; i < verts.size(); i++)
      bRes = bRes && (crosses[i] * normal > 0.0);
    //if it is internal point, we found nearest point, so return it
    if(bRes)
    {
      n = nearest;
      return true;
    }

    //else find nearest points on the elements of border
    for(i = 0; i < verts.size(); i++)
    {
      nearPoints[i] = edgevertex(verts[i], verts[(i + 1) % verts.size()], v);
    }
    //find nearest
    for(i = 0; i < verts.size(); i++)
    {
      curDist = sqrt(v | nearPoints[i]);
      if(i == 0 || dist > curDist)
      {
        dist = curDist;
        nMin  = i;
      }
    }
    //and return as result
    n = nearPoints[nMin];
    return false;
  }

}



//----------------------------------------------------------------------------
lhpOpLMProj::lhpOpLMProj(bool internalproj, const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType             = OPTYPE_OP;
  m_Canundo            = true;
  m_InputPreserving    = false; //Natural_preserving
  m_Source             = NULL;
  m_SourceName         = _R("none");
  m_MultiTime          = true;
  m_InternalProjection = internalproj;
}
//----------------------------------------------------------------------------
lhpOpLMProj::~lhpOpLMProj()
//----------------------------------------------------------------------------
{
  mafDEL(m_Output);
}
//----------------------------------------------------------------------------
mafOp* lhpOpLMProj::Copy()
//----------------------------------------------------------------------------
{
  lhpOpLMProj *cp = new lhpOpLMProj(m_InternalProjection, GetLabel());
  cp->m_Canundo   = m_Canundo;
  cp->m_OpType    = m_OpType;
  cp->SetListener(GetListener());
  cp->m_Next      = NULL;
  return cp;
}
//----------------------------------------------------------------------------
bool lhpOpLMProj::Accept(mafNode* node)   
//----------------------------------------------------------------------------
{
  return  (node && (node->IsMAFType(mafVMELandmarkCloud)));
}
//----------------------------------------------------------------------------
enum SURFACE_PROJ_ID
//----------------------------------------------------------------------------
{
  ID_CHOOSE = MINID,
  ID_MULTITIME,
};
//----------------------------------------------------------------------------
void lhpOpLMProj::OpRun()
//----------------------------------------------------------------------------
{  
  if(!m_TestMode)
  {
    // interface:
    m_Gui = new mafGUI(this);
    m_Gui->SetListener(this);
    m_Gui->Label(_L("Project to :"),true);
    m_Gui->Label(&m_SourceName);
    m_Gui->Button(ID_CHOOSE,_L("source "));
    //m_Gui->Bool(ID_MULTITIME,_("multi-time"),&m_MultiTime);
    m_Gui->Label(_R(""));
    m_Gui->OkCancel();
    m_Gui->Enable(wxOK, false);

    ShowGui();
  }

  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void lhpOpLMProj::OpDo()
//----------------------------------------------------------------------------
{

  std::vector<mafTimeStamp> stmps;
  mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Input);
  mafVMELandmarkCloud *newcloud;// = mafVMELandmarkCloud::New();
  mafString           ncname;

  if(m_Output)
    mafDEL(m_Output);
  stmps.clear();
  if(cloud->IsOpen())
  {
    mafVMELandmark *lm = cloud->GetLandmark(0);
    if(lm != NULL)
      lm->GetAbsTimeStamps(stmps);
  }
  else
  {
    cloud->GetAbsTimeStamps(stmps);
  }
  if(stmps.size() == 0)
  {
    mafEventMacro(mafEvent(this, OP_RUN_CANCEL));
    return;
  }

  ncname.Append(cloud->GetName());
  ncname.Append(_R("_projected_on_"));
  ncname.Append(m_SourceName);
  mafNEW(newcloud);
  m_Output = newcloud;
  newcloud->SetName(ncname);
  newcloud->SetNumberOfLandmarks(cloud->GetNumberOfLandmarks());
  for(int i = 0; i < cloud->GetNumberOfLandmarks(); i++)
  {
    newcloud->SetLandmarkName(i, cloud->GetLandmarkName(i));
  }
  m_Output->ReparentTo(m_Input->GetParent());

  mafVMEOutputSurface *surface = NULL;
  vtkMAFSmartPointer<vtkTriangleFilter> triangles;
  vtkMAFSmartPointer<vtkTransformPolyDataFilter> v_tpdf;
  vtkPolyData  *input = NULL;
  vtkPoints    *pts   = NULL;
  vtkCellArray *polys = NULL;
  vtkIdType npnts = 0;
  vtkIdType nclls = 0;

  if(m_Source != NULL)
  {
    surface = mafVMEOutputSurface::SafeDownCast(mafVME::SafeDownCast(m_Source)->GetOutput());
    surface->Update();
    triangles->SetInputConnection(surface->GetVTKOutputPort());
    triangles->Update();
    v_tpdf->SetInputConnection(triangles->GetOutputPort());
    v_tpdf->SetTransform(surface->GetAbsTransform()->GetVTKTransform());
    v_tpdf->Update();
    input = v_tpdf->GetOutput();
    polys = input->GetPolys();
    pts   = input->GetPoints();
    npnts = pts->GetNumberOfPoints();
    nclls = polys->GetNumberOfCells();
  }

  for(int i = 0; i < stmps.size(); i++)
  {
    std::vector<V3d<double> > coords;
    coords.resize(npnts);
    for(int j = 0; j < npnts; j++)
    {
      pts->GetPoint(j, coords[j].components);
    }

    vtkIdType npts = 0;
    vtkIdType *indx = 0;

    mafMatrix m, nm;
    cloud->GetOutput()->GetAbsMatrix(m, stmps[i]);
    newcloud->GetOutput()->GetAbsMatrix(nm, stmps[i]);
    m.SetTimeStamp(stmps[i]);
    nm.Invert();
    nm.SetTimeStamp(stmps[i]);
    //newcloud->SetMatrix(m);
    for(int j = 0; j < cloud->GetNumberOfLandmarks(); j++)
    {
      V3d<double> lm, nearest;
      bool   visible;
      cloud->GetLandmark(j, lm.components, stmps[i]);
      V4d<double> lm4, lm41;
      lm4 = V4d<double>(lm[0], lm[1], lm[2], 1.0);
      m.MultiplyPoint(lm4.components, lm41.components);
      lm = V3d<double>(lm41.components);

      if(m_Source != NULL)
      {
        bool inited = false;
        V3d<double> proj = lm;
        for (polys->InitTraversal(); polys->GetNextCell(npts, indx); )
        {
          std::vector<V3d<double> > poly;
          for(int jj = 0; jj < npts; jj++)
          {
            poly.push_back(coords[indx[jj]]);
          }
          bool inside = facevertex(poly, lm, nearest);
          if(m_InternalProjection)
          {
            if(inside)
            {
              proj = nearest;
              break;
            }
          }
          else if(!inited || ((lm | proj) < (lm | nearest)))
          {
            inited = true;
            proj = nearest;
          }
        }
        lm = proj;
      }
      lm4 = V4d<double>(lm[0], lm[1], lm[2], 1.0);
      nm.MultiplyPoint(lm4.components, lm41.components);
      lm = V3d<double>(lm41.components);
      newcloud->SetLandmark(j, lm[0], lm[1], lm[2], stmps[i]);
      visible = cloud->GetLandmarkVisibility(j, stmps[i]);
      newcloud->SetLandmarkVisibility(j, visible, stmps[i]);
    }
  }
  newcloud->Modified();
  newcloud->Update();
  //newcloud->Register(this);
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void lhpOpLMProj::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_Output)
  {
    mafEventMacro(mafEvent(this, VME_REMOVE, m_Output));
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
}

//----------------------------------------------------------------------------
void lhpOpLMProj::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  mafEventMacro(mafEvent(this,result));
}


//----------------------------------------------------------------------------
void lhpOpLMProj::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_MULTITIME:
        break;
      case ID_CHOOSE:
        {
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

          m_Source = mafVME::SafeDownCast(sel);
          SetNodeName(m_Source, &m_SourceName);
          m_Gui->Enable(wxOK, true);
          m_Gui->Update();
        }
        break;
      case wxOK:
        if(m_Source == NULL)
          break;
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
        default:
        mafEventMacro(*e);
      break; 
    }
  }  
}

//----------------------------------------------------------------------------
void lhpOpLMProj::SetNodeName(mafVME *pVME, mafString *pName) 
//----------------------------------------------------------------------------
{
  *pName = pVME->GetName();
  if(pVME->GetParent() != NULL)
  {
    *pName += _R(" parent:");
    *pName += pVME->GetParent()->GetName();
  }
}
